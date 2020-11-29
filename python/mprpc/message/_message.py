"""definition of messages and implementation of their parsing functions
"""

from collections import namedtuple


class Request(namedtuple('Request', ['msgtype', 'msgid', 'method', 'params'])):
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

    Attributes
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


class Response(namedtuple('Response', ['msgtype', 'msgid', 'error', 'result'])):
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

    Attributes
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


class Notification(namedtuple('Notification', ['msgtype', 'method', 'params'])):
    """class of notification messages

    Parameters
    ----------
    msgtype : mprpc.messages.MsgType
        message type
    method : str
        method name
    params : List[Any]
        parameters

    Attributes
    ----------
    msgtype : mprpc.messages.MsgType
        message type
    method : str
        method name
    params : List[Any]
        parameters
    """
