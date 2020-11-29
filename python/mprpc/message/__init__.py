"""module of message-related functions
"""

from .._mprpc_cpp import (
    MessageData,
)
from ._msgtype import MsgType
from ._msgid import MAX_MSGID, is_valid_msgid
from ._message import Request, Response, Notification
from ._message_validation import validate_message

for obj in (
    MessageData,
    MsgType,
    Request,
    Response,
    Notification,
    validate_message,
    is_valid_msgid,
):
    obj.__module__ = "mprpc.message"
del obj
