"""module of message-related functions
"""

from .._mprpc_cpp import (
    MessageData,
)

for obj in (
    MessageData,
):
    obj.__module__ = "mprpc.message"
del obj
