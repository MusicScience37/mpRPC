"""implementation of FunctionMethodExecutor class
"""

from typing import Callable, Optional, List
import logging

import marshmallow

from mprpc.transport import Session
from mprpc.message import (
    Request, Notification, MessageData, validate_data_list, pack_response,
)

from ._method_executor import MethodExecutor


class FunctionMethodExecutor(MethodExecutor):
    """class of executors of methods written in functions

    Parameters
    ----------
    name : str
        method name
    function : Callable
        function to call for method execution
    param_schemas : Optional[List[Optional[marshmallow.Schema]]], optional
        list of schemas for parameters, by default None to specify no validation.
        If a list is given, each element in the list is assumed to be None (no validation) or
        an object of a class inherited from marshmallow.Schema to use for validation.
    result_schema : Optional[marshmallow.Schema]
        schema for result of method, by default None to specify no serialization.
        If a schema is given, results of method executions for requests are serialized
        using this schema.
    """

    def __init__(self, name: str, function: Callable, *,
                 param_schemas: Optional[List[Optional[marshmallow.Schema]]] = None,
                 result_schema: Optional[marshmallow.Schema] = None):
        super().__init__(name)
        self.__function = function
        self.__param_schemas = param_schemas
        self.__result_schema = result_schema
        self.__logger = logging.getLogger('mprpc.method.' + name)

    @property
    def function(self) -> Callable:
        """get function

        Returns
        -------
        Callable
            function to call for method execution
        """

        return self.__function

    def process_request(self, session: Session, msg: Request) -> MessageData:
        """process a request

        Parameters
        ----------
        session : Session
            session
        msg : Request
            request message

        Returns
        -------
        MessageData
            response message data
        """

        try:
            params = validate_data_list(msg.params, self.__param_schemas)
            result = self.__function(*params)
            if self.__result_schema:
                result = self.__result_schema.dump(result)
            return pack_response(msg.msgid, result=result)
        except Exception as err:  # pylint: disable=broad-except
            # all exceptions, which may be thrown by user cades, must be processed.
            self.__logger.error('exception thrown in method: %s', err)
            return pack_response(msg.msgid, error=str(err))

    def process_notification(self, session: Session, msg: Notification):
        """process a notification

        Parameters
        ----------
        session : Session
            session
        msg : Notification
            notification message
        """

        try:
            params = validate_data_list(msg.params, self.__param_schemas)
            self.__function(*params)
        except Exception as err:  # pylint: disable=broad-except
            # all exceptions, which may be thrown by user cades, must be processed.
            self.__logger.error('exception thrown in method: %s', err)
