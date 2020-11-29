"""implementation of functions to serialize / deserialize data
"""

from typing import Any
from msgpack import packb, unpackb

from mprpc import MPRPCException, ErrorInfo, ErrorCode
from .._mprpc_cpp import MessageData


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
