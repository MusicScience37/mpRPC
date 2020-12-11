"""module of server classes
"""

from .._mprpc_cpp import (
    PythonServerHelper,
)

for obj in (
    PythonServerHelper,
):
    obj.__module__ = "mprpc.server"
del obj
