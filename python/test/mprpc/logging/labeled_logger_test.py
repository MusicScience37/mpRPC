"""test of LabeledLogger class
"""

from _mprpc_cpp import PythonLoggerHelper
from mprpc.logging import LabeledLogger, LogLevel


class MockLogger(PythonLoggerHelper):  # pylint: disable=too-few-public-methods
    """logger class for test
    """

    def __init__(self):
        super().__init__(self.__write_impl, LogLevel.TRACE)
        self.label = None
        self.filename = None
        self.line = None
        self.function = None
        self.log_level = None
        self.message = None

    # pylint: disable=too-many-arguments
    def __write_impl(self, label: str, filename: str, line: int, function: str,
                     log_level: LogLevel, message: str):
        """write log
        """

        self.label = label
        self.filename = filename
        self.line = line
        self.function = function
        self.log_level = log_level
        self.message = message


def test_labeled_logger_from_logger_without_label():
    """test of LabeledLogger created from logger without label
    """

    origin = MockLogger()
    logger = LabeledLogger(origin)

    assert logger.logger == origin
    assert logger.label == 'mprpc'

    logger.trace('test %s', 'log')
    assert origin.label == 'mprpc'
    assert origin.filename == __file__
    assert origin.line == 45
    assert origin.function == 'test_labeled_logger_from_logger_without_label'
    assert origin.log_level == LogLevel.TRACE
    assert origin.message == 'test log'

    logger.debug('test %s', 'log')
    assert origin.log_level == LogLevel.DEBUG

    logger.info('test %s', 'log')
    assert origin.log_level == LogLevel.INFO

    logger.warn('test %s', 'log')
    assert origin.log_level == LogLevel.WARN

    logger.error('test %s', 'log')
    assert origin.log_level == LogLevel.ERROR

    logger.fatal('test %s', 'log')
    assert origin.log_level == LogLevel.FATAL


def test_labeled_logger_from_logger_with_label():
    """test of LabeledLogger created from logger with label
    """

    origin = MockLogger()
    label = 'test_logger'
    logger = LabeledLogger(origin, label)

    assert logger.logger == origin
    assert logger.label == label

    logger.trace('test %s', 'log')
    assert origin.label == label


def test_labeled_logger_from_labeled_logger_without_label():
    """test of LabeledLogger created from LabeledLogger object without label
    """

    origin = MockLogger()
    label = 'test_logger'
    logger = LabeledLogger(LabeledLogger(origin, label))

    assert logger.logger == origin
    assert logger.label == label

    logger.trace('test %s', 'log')
    assert origin.label == label


def test_labeled_logger_from_labeled_logger_with_label():
    """test of LabeledLogger created from LabeledLogger object with label
    """

    origin = MockLogger()
    label1 = 'test_logger'
    label2 = 'test'
    logger = LabeledLogger(LabeledLogger(origin, label1), label2)

    label = label1 + '.' + label2

    assert logger.logger == origin
    assert logger.label == label

    logger.trace('test %s', 'log')
    assert origin.label == label
