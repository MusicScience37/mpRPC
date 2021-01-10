"""test of PythonLogger class
"""

import logging
from mprpc.logging import LogLevel, PythonLogger


def test_python_logger(caplog):
    """test of PythonLogger class
    """

    logger = PythonLogger()
    logger.write("mprpc.test_python_logger", "python_logger_test.py",
                 1, "test_python_logger", LogLevel.WARN, "warning message")
    assert caplog.record_tuples == [
        ("mprpc.test_python_logger", logging.WARN, "warning message")]
    caplog.clear()
