"""implementation of MethodServer class
"""

from abc import ABC, abstractmethod
from typing import Callable, Any

from mprpc.transport import Session
from mprpc.message import MessageData
from mprpc import ErrorInfo

# This file ignores too-few-public-methods warning to define interface


class MethodServer(ABC):  # pylint: disable=too-few-public-methods
    """base class of servers to execute methods
    """

    @abstractmethod
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
