"""functions to convert log levels
"""

import logging

from .._mprpc_cpp import LogLevel

PYTHON_TO_MPRPC_DICT = {
    logging.DEBUG: LogLevel.DEBUG,
    logging.INFO: LogLevel.INFO,
    logging.WARNING: LogLevel.WARN,
    logging.ERROR: LogLevel.ERROR,
    logging.CRITICAL: LogLevel.FATAL,
}


def to_mprpc_log_level(level: int) -> LogLevel:
    """convert log level from Python's logging to mpRPC

    Parameters
    ----------
    level : int
      log level in Python's logging module

    Returns
    -------
    mprpc.logging.LogLevel
      converted log level
    """

    return PYTHON_TO_MPRPC_DICT[level]


MPRPC_TO_PYTHON_DICT = {
    LogLevel.TRACE: logging.DEBUG,
    LogLevel.DEBUG: logging.DEBUG,
    LogLevel.INFO: logging.INFO,
    LogLevel.WARN: logging.WARN,
    LogLevel.ERROR: logging.ERROR,
    LogLevel.FATAL: logging.CRITICAL,
}


def to_python_log_level(level: LogLevel) -> int:
    """convert log level from mpRPC to Python's logging

    Parameters
    ----------
    mprpc.logging.LogLevel
      log level in mpRPC

    Returns
    -------
    int
      converted log level in Python's logging
    """

    return MPRPC_TO_PYTHON_DICT[level]
