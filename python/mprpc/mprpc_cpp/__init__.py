"""package to import Python wrapper of mpRPC C++ library
"""
from _mprpc_cpp import (
    LogLevel,
    Logger,
    PythonLoggerHelper,
    create_file_logger,
    create_stdout_logger,
)

for obj in (
    LogLevel,
    Logger,
    PythonLoggerHelper,
    create_file_logger,
    create_stdout_logger,
):
    obj.__module__ = "mprpc.mprpc_cpp"
