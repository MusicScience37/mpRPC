"""An implementation of MessagePack-RPC

Submodules
----------
- :py:mod:`mprpc.logging`
- :py:mod:`mprpc.message`
"""

from ._mprpc_cpp import (
    ErrorInfo,
    MPRPCException,
)

from ._error_code import ErrorCode

for obj in (
    ErrorCode,
    ErrorInfo,
    MPRPCException,
):
    obj.__module__ = "mprpc"
del obj
