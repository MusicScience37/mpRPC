"""An implementation of MessagePack-RPC
"""

from ._mprpc_cpp import (
    ErrorInfo,
    MPRPCException,
)

for obj in (
    ErrorInfo,
    MPRPCException,
):
    obj.__module__ = "mprpc"
del obj
