"""module of message-related functions
"""

from .._mprpc_cpp import (
    MessageData,
)
from ._msgtype import MsgType
from ._msgid import MAX_MSGID, is_valid_msgid
from ._message import Request, Response, Notification
from ._message_validation import validate_message
from ._data_validation import validate_data, validate_data_list
from ._serialization import (
    pack_object, unpack_object, pack_request, pack_response, pack_notification
)

for obj in (
    MessageData,
    MsgType,
    Request,
    Response,
    Notification,
    validate_message,
    is_valid_msgid,
    validate_data,
    validate_data_list,
    pack_object,
    unpack_object,
    pack_object,
    unpack_object,
    pack_request,
    pack_response,
    pack_notification,
):
    obj.__module__ = "mprpc.message"
del obj
