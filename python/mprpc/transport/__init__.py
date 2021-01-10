"""module of data transport
"""

from .._mprpc_cpp import (
    Address,
    Session,
)

for obj in (
    Address,
    Session,
):
    obj.__module__ = "mprpc.transport"
del obj
