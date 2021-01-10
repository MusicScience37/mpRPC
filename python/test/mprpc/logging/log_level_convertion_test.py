"""test of mprpc.logging.log_level_convertion
"""

import logging
from mprpc.logging import LogLevel, to_mprpc_log_level, to_python_log_level


def test_to_mprpc_log_level():
    """test of mprpc.logging.log_level_convertion.to_mprpc_log_level
    """

    assert to_mprpc_log_level(logging.DEBUG) == LogLevel.DEBUG
    assert to_mprpc_log_level(logging.INFO) == LogLevel.INFO
    assert to_mprpc_log_level(logging.WARNING) == LogLevel.WARN
    assert to_mprpc_log_level(logging.ERROR) == LogLevel.ERROR
    assert to_mprpc_log_level(logging.CRITICAL) == LogLevel.FATAL


def test_to_python_log_level():
    """test of mprpc.logging.log_level_convertion.to_python_log_level
    """

    assert to_python_log_level(LogLevel.TRACE) == logging.DEBUG
    assert to_python_log_level(LogLevel.DEBUG) == logging.DEBUG
    assert to_python_log_level(LogLevel.INFO) == logging.INFO
    assert to_python_log_level(LogLevel.WARN) == logging.WARNING
    assert to_python_log_level(LogLevel.ERROR) == logging.ERROR
    assert to_python_log_level(LogLevel.FATAL) == logging.CRITICAL
