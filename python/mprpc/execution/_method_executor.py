"""implementation of MethodExecutor class
"""

from abc import ABC, abstractmethod
from mprpc.transport import Session
from mprpc.message import Request, Notification, MessageData


class MethodExecutor(ABC):
    """base class of executors of methods

    Parameters
    ----------
    name : str
        method name
    """

    def __init__(self, name: str):
        self.__name = name

    @property
    def name(self) -> str:
        """get method name

        Returns
        -------
        str
            method name
        """

        return self.__name

    @abstractmethod
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

    @abstractmethod
    def process_notification(self, session: Session, msg: Notification):
        """process a notification

        Parameters
        ----------
        session : Session
            session
        msg : Notification
            notification message
        """
