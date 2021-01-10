"""module to execute methods
"""

from ._method_executor import MethodExecutor
from ._function_method_executor import FunctionMethodExecutor
from ._method_server import MethodServer
from ._simple_method_server import SimpleMethodServer

for obj in (
    MethodExecutor,
    FunctionMethodExecutor,
    MethodServer,
    SimpleMethodServer,
):
    obj.__module__ = "mprpc.execution"
del obj
