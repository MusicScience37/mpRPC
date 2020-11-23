"""package to import Python wrapper of mpRPC C++ library
"""
from _mprpc_cpp import (
    LogLevel,
    Logger,
    PythonLoggerHelper,
    FileLogger,
    StdoutLogger,
)

for obj in (
    LogLevel,
    Logger,
    PythonLoggerHelper,
    FileLogger,
    StdoutLogger,
):
    obj.__module__ = "mprpc.mprpc_cpp"
