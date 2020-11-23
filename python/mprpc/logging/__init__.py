"""module to write logs
"""

from .._mprpc_cpp import (
    LogLevel,
    Logger,
    PythonLoggerHelper,
    create_file_logger,
    create_stdout_logger,
)
from .python_logger import PythonLogger

for obj in (
    LogLevel,
    Logger,
    PythonLoggerHelper,
    create_file_logger,
    create_stdout_logger,
):
    obj.__module__ = "mprpc.logging"
del obj
