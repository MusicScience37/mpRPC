"""module of client classes
"""

from .._mprpc_cpp import (
    PythonClientHelper,
)
from ._client import Client

for obj in (
    PythonClientHelper,
    Client,
):
    obj.__module__ = "mprpc.client"
del obj
