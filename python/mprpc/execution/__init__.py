"""module to execute methods
"""

from ._method_executor import MethodExecutor
from ._function_method_executor import FunctionMethodExecutor
from ._method_server import MethodServer

for obj in (
    MethodExecutor,
    FunctionMethodExecutor,
    MethodServer,
):
    obj.__module__ = "mprpc.execution"
del obj
