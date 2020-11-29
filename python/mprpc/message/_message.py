"""definition of messages and implementation of their parsing functions
"""

from collections import namedtuple
from typing import List, Any, Union

from mprpc import ErrorInfo, MPRPCException, ErrorCode

from ._msgtype import MsgType
from ._msgid import is_valid_msgid


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


def _make_parse_error(message: str, data: Any):
    """raise an exception for a parse error

    Parameters
    ----------
    message : str
        error message
    data : Any
        data

    Returns
    -------
    MPRPCException
        exception object to raise
    """

    return MPRPCException(
        ErrorInfo(ErrorCode.PARSE_ERROR, '%s: %s' % (message, data)))


def _validate_msgid(msgid: Any, data: Any):
    """validate message ID

    Parameters
    ----------
    msgid : Any
        message ID
    data : Any
        data (for exception)

    Raises
    ------
    mprpc.MPRPCException
        If invalid.
    """

    if not isinstance(msgid, int):
        raise _make_parse_error('message ID must be an integer', data)
    if not is_valid_msgid(msgid):
        raise _make_parse_error('invalid message ID value %s' % msgid, data)


def _validate_method(method: Any, data: Any):
    """validate method name

    Parameters
    ----------
    method : Any
        method name
    data : Any
        data (for exception)

    Raises
    ------
    mprpc.MPRPCException
        If invalid.
    """

    if not isinstance(method, str):
        raise _make_parse_error('method name must be a string', data)


def _validate_params(params: Any, data: Any):
    """validate parameters

    Parameters
    ----------
    params : Any
        parameters
    data : Any
        data (for exception)

    Raises
    ------
    mprpc.MPRPCException
        If invalid.
    """

    if not isinstance(params, list):
        raise _make_parse_error('parameters must be a list', data)


def _parse_request(data: List[Any]) -> Request:
    """parse request message

    Parameters
    ----------
    data : List
        data

    Returns
    -------
    Request
        parsed message

    Raises
    ------
    mprpc.MPRPCException
        If data is invalid.

    Notes
    -----
    This function assumes that the first element of data has already been validated.
    """

    if len(data) != 4:
        raise _make_parse_error(
            'request message must have four elements', data)

    msgtype = MsgType.REQUEST

    msgid = data[1]
    _validate_msgid(msgid, data)

    method = data[2]
    _validate_method(method, data)

    params = data[3]
    _validate_params(params, data)

    return Request(msgtype, msgid, method, params)


def _parse_response(data: List) -> Response:
    """parse response message

    Parameters
    ----------
    data : List
        data

    Returns
    -------
    Response
        parsed message

    Raises
    ------
    mprpc.MPRPCException
        If data is invalid.

    Notes
    -----
    This function assumes that the first element of data has already been validated.
    """

    if len(data) != 4:
        raise _make_parse_error(
            'response message must have four elements', data)

    msgtype = MsgType.RESPONSE

    msgid = data[1]
    _validate_msgid(msgid, data)

    error = data[2]

    result = data[3]

    return Response(msgtype, msgid, error, result)


def _parse_notification(data: List) -> Notification:
    """parse notification message

    Parameters
    ----------
    data : List
        data

    Returns
    -------
    Notification
        parsed message

    Raises
    ------
    mprpc.MPRPCException
        If data is invalid.

    Notes
    -----
    This function assumes that the first element of data has already been validated.
    """

    if len(data) != 3:
        raise _make_parse_error(
            'notification message must have four elements', data)

    msgtype = MsgType.NOTIFICATION

    method = data[1]
    _validate_method(method, data)

    params = data[2]
    _validate_params(params, data)

    return Notification(msgtype, method, params)


def parse_message(data: Any) -> Union[Request, Response, Notification]:
    """parse message data

    Parameters
    ----------
    data : Any
        message data, using fundamental types in Python (list, int, str, ...).

    Returns
    -------
    Union[Request, Response, Notification]
        parsed message

    Raises
    ------
    mprpc.MPRPCException
        If data is invalid.
    """

    if not isinstance(data, list):
        raise _make_parse_error('message must be a list', data)
    if len(data) == 0:
        raise _make_parse_error(
            'message must have three or four elements', data)
    if not isinstance(data[0], int):
        raise _make_parse_error(
            'the first element in the message must an integer', data)
    msgtype_int = data[0]

    if msgtype_int == int(MsgType.REQUEST):
        return _parse_request(data)
    if msgtype_int == int(MsgType.RESPONSE):
        return _parse_response(data)
    if msgtype_int == int(MsgType.NOTIFICATION):
        return _parse_notification(data)

    raise _make_parse_error('invalid message type %d' % msgtype_int, data)
