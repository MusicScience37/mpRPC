"""module of message-related functions
"""

from .._mprpc_cpp import (
    MessageData,
)
from ._msgtype import MsgType

for obj in (
    MessageData,
    MsgType,
):
    obj.__module__ = "mprpc.message"
del obj
