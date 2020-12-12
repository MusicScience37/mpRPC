"""module to execute methods
"""

from ._method_executor import MethodExecutor
from ._method_server import MethodServer

for obj in (
    MethodExecutor,
    MethodServer,
):
    obj.__module__ = "mprpc.execution"
del obj
