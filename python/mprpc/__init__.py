"""An implementation of MessagePack-RPC
"""

from ._mprpc_cpp import (
    ErrorInfo,
)

for obj in (
    ErrorInfo,
):
    obj.__module__ = "mprpc"
del obj
