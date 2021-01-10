"""implementation of LabeledLogger class
"""

import inspect
from typing import Union, Optional

from .._mprpc_cpp import Logger, LogLevel


class LabeledLogger:
    """class of loggers with labels

    Parameters
    ----------
    logger : Union[Logger, LabeledLogger]
        logger
    label : Optional[str]
        label
    """

    def __init__(self, logger: Union[Logger, 'LabeledLogger'], label: Optional[str] = None):
        if isinstance(logger, LabeledLogger):
            self.__logger = logger.logger
            self.__label = logger.label
            if label:
                self.__label = self.__label + '.' + label
        else:
            self.__logger = logger
            if label:
                self.__label = label
            else:
                self.__label = 'mprpc'

    @property
    def logger(self) -> Logger:
        """get logger

        Returns
        -------
        Logger
            logger
        """

        return self.__logger

    @property
    def label(self) -> str:
        """get label

        Returns
        -------
        str
            label
        """

        return self.__label

    def _write_impl(self, log_level: LogLevel, format_str: str, *args):
        """write log

        Parameters
        ----------
        log_level : LogLevel
            log level
        format_str : str
            format string, used as ``format_str % args``
        """

        caller_frame: inspect.FrameInfo = inspect.stack()[2]
        filename = caller_frame.filename
        line = caller_frame.lineno
        function = caller_frame.function
        message = format_str % args
        self.__logger.write(self.__label, filename, line,
                            function, log_level, message)

    def trace(self, format_str: str, *args):
        """write trace log

        Parameters
        ----------
        format_str : str
            format string, used as ``format_str % args``
        """

        self._write_impl(LogLevel.TRACE, format_str, *args)

    def debug(self, format_str: str, *args):
        """write debug log

        Parameters
        ----------
        format_str : str
            format string, used as ``format_str % args``
        """

        self._write_impl(LogLevel.DEBUG, format_str, *args)

    def info(self, format_str: str, *args):
        """write info log

        Parameters
        ----------
        format_str : str
            format string, used as ``format_str % args``
        """

        self._write_impl(LogLevel.INFO, format_str, *args)

    def warn(self, format_str: str, *args):
        """write warning log

        Parameters
        ----------
        format_str : str
            format string, used as ``format_str % args``
        """

        self._write_impl(LogLevel.WARN, format_str, *args)

    def error(self, format_str: str, *args):
        """write error log

        Parameters
        ----------
        format_str : str
            format string, used as ``format_str % args``
        """

        self._write_impl(LogLevel.ERROR, format_str, *args)

    def fatal(self, format_str: str, *args):
        """write fatal log

        Parameters
        ----------
        format_str : str
            format string, used as ``format_str % args``
        """

        self._write_impl(LogLevel.FATAL, format_str, *args)
