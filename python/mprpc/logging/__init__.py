"""module to write logs
"""

from .._mprpc_cpp import (
    LogLevel,
    Logger,
    create_file_logger,
    create_stdout_logger,
)
from ._log_level_convertion import to_mprpc_log_level, to_python_log_level
from ._python_logger import PythonLogger
from ._labeled_logger import LabeledLogger

for obj in (
    LogLevel,
    Logger,
    PythonLogger,
    LabeledLogger,
    create_file_logger,
    create_stdout_logger,
):
    obj.__module__ = "mprpc.logging"
del obj
