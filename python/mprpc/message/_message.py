"""definition of messages and implementation of their parsing functions
"""

from typing import NamedTuple, Any, List

from ._msgtype import MsgType


# pylint: disable=too-few-public-methods
class Request(NamedTuple):
    """class of request messages

    Parameters
    ----------
    msgtype : mprpc.messages.MsgType
        message type
    msgid : int
        message ID
    method : str
        method name
    params : List[Any]
        parameters
    """

    msgtype: MsgType  # message type
    msgid: int  # message ID
    method: str  # method name
    params: List[Any]  # parameters


class Response(NamedTuple):
    """class of response messages

    Parameters
    ----------
    msgtype : mprpc.messages.MsgType
        message type
    msgid : int
        message ID
    error : Any
        error
    result : Any
        result
    """

    msgtype: MsgType  # message type
    msgid: int  # message ID
    error: Any  # error
    result: Any  # result


class Notification(NamedTuple):
    """class of notification messages

    Parameters
    ----------
    msgtype : mprpc.messages.MsgType
        message type
    method : str
        method name
    params : List[Any]
        parameters
    """

    msgtype: MsgType  # message type
    method: str  # method name
    params: List[Any]  # parameters
