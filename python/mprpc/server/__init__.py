"""module of server classes
"""

from .._mprpc_cpp import (
    PythonServerHelper,
)
from ._server import Server

for obj in (
    PythonServerHelper,
    Server,
):
    obj.__module__ = "mprpc.server"
del obj
