"""implementation of functions to serialize / deserialize data
"""

from typing import Any, List
from msgpack import packb, unpackb

from mprpc import MPRPCException, ErrorInfo, ErrorCode
from .._mprpc_cpp import MessageData
from ._msgtype import MsgType


def pack_object(obj: Any) -> MessageData:
    """pack an object

    This function calls msgpack.packb function.

    Parameters
    ----------
    obj : Any
        object to pack

    Returns
    -------
    mprpc.message.MessageData
        packed bytes

    Raises
    ------
    mprpc.MPRPCException
        If msgpack package failed to pack data.
    """

    try:
        return MessageData(packb(obj))
    except Exception as err:
        raise MPRPCException(
            ErrorInfo(ErrorCode.UNEXPECTED_ERROR, 'error in packing data: %s' % obj)) from err


def unpack_object(data: MessageData) -> Any:
    """unpack data

    Parameters
    ----------
    data : mprpc.message.MessageData
        data to unpack

    Returns
    -------
    Any
        unpacked data

    Raises
    ------
    mprpc.MPRPCException
        If msgpack package failed to unpack data.
    """

    try:
        return unpackb(bytes(data))
    except Exception as err:
        raise MPRPCException(
            ErrorInfo(ErrorCode.PARSE_ERROR, 'error in unpacking data', data)) from err


def pack_request(msgid: int, method: str, params: List[Any]) -> MessageData:
    """pack request message

    Parameters
    ----------
    msgid : int
        message ID
    method : str
        method name
    params : List[Any]
        parameters

    Returns
    -------
    MessageData
        packed data

    Raises
    ------
    mprpc.MPRPCException
        If msgpack package failed to pack data.
    """

    return pack_object([int(MsgType.REQUEST), msgid, method, params])


def pack_response(msgid: int, error: Any = None, result: Any = None) -> MessageData:
    """pack response message

    Parameters
    ----------
    msgid : int
        message ID
    error : Any, optional
        error, by default None
    result : Any, optional
        result, by default None

    Returns
    -------
    MessageData
        packed data

    Raises
    ------
    mprpc.MPRPCException
        If msgpack package failed to pack data.
    """

    return pack_object([int(MsgType.RESPONSE), msgid, error, result])


def pack_notification(method: str, params: Any) -> MessageData:
    """pack notification message

    Parameters
    ----------
    method : str
        method name
    params : List[Any]
        parameters

    Returns
    -------
    MessageData
        packed data

    Raises
    ------
    mprpc.MPRPCException
        If msgpack package failed to pack data.
    """

    return pack_object([int(MsgType.NOTIFICATION), method, params])
