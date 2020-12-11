"""module of client classes
"""

from .._mprpc_cpp import (
    PythonClientHelper,
)

for obj in (
    PythonClientHelper,
):
    obj.__module__ = "mprpc.client"
del obj
