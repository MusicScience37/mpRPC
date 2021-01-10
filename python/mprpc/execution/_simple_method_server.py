"""implementation SimpleMethodServer
"""


from typing import Callable, Any, List
from concurrent.futures import ThreadPoolExecutor, Future

from mprpc.logging import Logger, LabeledLogger
from mprpc.transport import Session
from mprpc.message import (
    Request, Notification, MessageData, pack_response, unpack_object, validate_message
)
from mprpc import ErrorInfo, ErrorCode, MPRPCException

from ._method_server import MethodServer
from ._method_executor import MethodExecutor


class SimpleMethodServer(MethodServer):
    """class of a simple server implementation to execute methods

    Parameters
    ----------
    logger : Logger
        logger
    num_threads : int
        number of threads
    methods : List[MethodExecutor]
        list of methods
    """

    def __init__(self, logger: Logger, methods: List[MethodExecutor]):
        self.__logger = LabeledLogger(logger, 'mprpc.simple_method_server')
        self.__methods = {
            method.name: method
            for method in methods
        }
        self.__threads = ThreadPoolExecutor(1)

    def async_process_message(self, session: Session, msg: MessageData,
                              handler: Callable[[ErrorInfo, bool, MessageData], Any]):
        """asynchronously process message

        Parameters
        ----------
        session : Session
            session
        msg : MessageData
            message
        handler : Callable[[ErrorInfo, bool, MessageData], Any]
            handler on message processed
            Parameters are error, flag whether a response exists, and response data (if exists).
        """

        def done_callback(future: Future):
            """callback called after message is precessed

            Parameters
            ----------
            future : Future
                future
            """

            nonlocal handler

            error = ErrorInfo()
            response_data = None

            try:
                response_data = future.result()
            except MPRPCException as err:
                error = err.args[0]

            has_response = response_data is not None
            if not has_response:
                response_data = MessageData()

            handler(error, has_response, response_data)

        self.__threads.submit(self.process_message, session,
                              msg).add_done_callback(done_callback)

    def process_message(self, session: Session, data: MessageData) -> MessageData:
        """process a message

        Parameters
        ----------
        session : Session
            session
        data : MessageData
            message

        Returns
        -------
        MessageData
            response data (if exists).
        """

        try:
            msg = validate_message(unpack_object(data))

            if isinstance(msg, Request):
                if msg.method not in self.__methods:
                    self.__logger.error('method %s not found', msg.method)
                    return pack_response(
                        msg.msgid, error='method {} not found'.format(msg.method))

                return self.__methods[msg.method].process_request(
                    session, msg)

            if isinstance(msg, Notification):
                if msg.method not in self.__methods:
                    self.__logger.error('method %s not found', msg.method)
                    return None

                self.__methods[msg.method].process_notification(session, msg)
                return None

            raise MPRPCException(
                ErrorInfo(ErrorCode.INVALID_MESSAGE, 'message must be a request or response'))
        except MPRPCException as err:
            self.__logger.error('%s', err)
            raise
        except Exception as err:
            self.__logger.error('%s', err)
            raise MPRPCException(
                ErrorInfo(ErrorCode.UNEXPECTED_ERROR, 'error in server: {}'.format(err))) from err
