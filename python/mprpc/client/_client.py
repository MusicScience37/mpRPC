"""implementation of Client class
"""

from threading import Lock
from concurrent.futures import Future
from typing import Any

from mprpc.logging import Logger, LabeledLogger
from mprpc.config import ClientConfig
from mprpc.message import (
    MessageData, pack_request, pack_notification,
    unpack_object, validate_message, Response,
)
from mprpc import ErrorInfo, ErrorCode, MPRPCException
from .._mprpc_cpp import PythonClientHelper


class ServerError(Exception):
    """exception for server-side errors
    """


class Client:
    """class of clients

    logger : Logger
        logger
    config : ClientConfig
        client configuration
    """

    def __init__(self, logger: Logger, config: ClientConfig):
        self.__impl = PythonClientHelper(logger, config, self._process_message)
        self.__logger = LabeledLogger(logger, 'mprpc.client')
        self.__sync_request_timeout_ms = config.sync_request_timeout_ms

        self.__lock = Lock()
        self.__requests = dict()
        self.__next_msgid = 0

    def start(self):
        """start process
        """

        self.__impl.start()

    def stop(self):
        """stop process
        """

        self.__impl.stop()

    def async_request(self, method: str, *args) -> Future:
        """asynchronously request

        Parameters
        ----------
        method : str
            method, remaining arguments are parameters to the method

        Returns
        -------
        Future
            future
        """

        with self.__lock:
            msgid = self.__next_msgid
            data = pack_request(msgid, method, args)
            future = Future()
            self.__impl.async_send(data, self._on_request_sent)
            future.set_running_or_notify_cancel()
            self.__next_msgid += 1
            self.__requests[msgid] = future

        return future

    def _on_request_sent(self, err: ErrorInfo):
        """process on a request sent

        Parameters
        ----------
        err : ErrorInfo
            error
        """

        if not err:
            return

        self.__logger.error('%s', err)
        with self.__lock:
            for future in self.__requests.values():
                future.set_exception(MPRPCException(err))
            self.__requests = dict()

    def _process_message(self, err: ErrorInfo, data: MessageData):
        """process a message

        Parameters
        ----------
        err : ErrorInfo
            error
        data : MessageData
            message data
        """

        try:
            if err:
                raise MPRPCException(err)

            msg = validate_message(unpack_object(data))
            if not isinstance(msg, Response):
                raise MPRPCException(ErrorInfo(
                    ErrorCode.INVALID_MESSAGE, 'received a message which is not a response'))

            msgid = msg.msgid
            with self.__lock:
                if msgid not in self.__requests:
                    self.__logger.warn(
                        'invalid msgid in received response: %d', msgid)
                future = self.__requests[msgid]
                del self.__requests[msgid]

            if msg.error:
                future.set_exception(ServerError(msg.error))
                return

            future.set_result(msg.result)

        except MPRPCException as exc:
            self.__logger.error('%s', exc)
            with self.__lock:
                for future in self.__requests.values():
                    future.set_exception(MPRPCException(err))
                self.__requests = dict()
            return

    def request(self, method: str, *args) -> Any:
        """request

        Parameters
        ----------
        method : str
            method, remaining arguments are parameters to the method

        Returns
        -------
        Any
            result
        """

        timeout = self.__sync_request_timeout_ms * 1e-3
        return self.async_request(method, *args).result(timeout=timeout)

    def notify(self, method: str, *args):
        """notify

        Parameters
        ----------
        method : str
            method, remaining arguments are parameters to the method
        """

        data = pack_notification(method, args)
        self.__impl.async_send(data, self._on_notification_sent)

    def _on_notification_sent(self, err: ErrorInfo):
        """process on notification sent

        Parameters
        ----------
        err : ErrorInfo
            error
        """

        if not err:
            return

        self.__logger.error('%s', err)
