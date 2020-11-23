"""implementation of PythonLogger class
"""

import logging

from .log_level_convertion import to_python_log_level
from .._mprpc_cpp import PythonLoggerHelper, LogLevel


class PythonLogger(PythonLoggerHelper):
    """class to write logs using Python's logging module
    """

    def __init__(self, log_output_level: LogLevel = LogLevel.WARN):
        """construct
        """

        super().__init__(self.__write_impl, log_output_level)
        self.logger = logging.getLogger("mprpc")

    # pylint: disable=too-many-arguments
    def __write_impl(self, label: str, filename: str, line: int, function: str,
                     log_level: LogLevel, message: str):
        """write log

        Parameters
        ----------
        label : str
          label
        filename : str
          file name
        line : int
          line number
        function : str
          function name
        log_level : LogLevel
          log level
        message : str
          log message

        Returns
        -------
        None
          none
        """

        log = logging.LogRecord(label, to_python_log_level(
            log_level), filename, line, message, [], None, function, None)
        self.logger.handle(log)
