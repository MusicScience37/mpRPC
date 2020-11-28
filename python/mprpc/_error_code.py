"""definition of ErrorCode enumeration
"""

from enum import IntEnum
from ._mprpc_cpp import ErrorCode as CppErrorCode


class ErrorCode(IntEnum):
    """enumeration of error codes

    Integer value of this enumeration is used in mprpc.ErrorInfo.code property.

    Attributes
    ----------
    SUCCESS : int
        success (no error), this value is zero.
    UNEXPECTED_ERROR : int
        unexpected error
    UNEXPECTED_NULLPTR : int
        unexpected null pointer
    PARSE_ERROR : int
        parse error
    INVALID_MESSAGE : int
        invalid message
    EOF : int
        end of file
    FAILED_TO_LISTEN : int
        failed to listen to an address
    FAILED_TO_ACCEPT : int
        failed to accept a connection
    FAILED_TO_RESOLVE : int
        failed to resolve a server
    FAILED_TO_CONNECT : int
        failed to connect to a server
    FAILED_TO_READ : int
        failed to read data
    FAILED_TO_WRITE : int
        failed to write data
    METHOD_NOT_FOUND : int
        method not found
    INVALID_FUTURE_USE : int
        use of invalid future
    INVALID_CONFIG_VALUE : int
        invalid configuration value
    CONFIG_PARSE_ERROR : int
        parse error of configuration files
    TIMEOUT : int
        timeout
    """

    SUCCESS = CppErrorCode.SUCCESS
    UNEXPECTED_ERROR = CppErrorCode.UNEXPECTED_ERROR
    UNEXPECTED_NULLPTR = CppErrorCode.UNEXPECTED_NULLPTR
    PARSE_ERROR = CppErrorCode.PARSE_ERROR
    INVALID_MESSAGE = CppErrorCode.INVALID_MESSAGE
    EOF = CppErrorCode.EOF
    FAILED_TO_LISTEN = CppErrorCode.FAILED_TO_LISTEN
    FAILED_TO_ACCEPT = CppErrorCode.FAILED_TO_ACCEPT
    FAILED_TO_RESOLVE = CppErrorCode.FAILED_TO_RESOLVE
    FAILED_TO_CONNECT = CppErrorCode.FAILED_TO_CONNECT
    FAILED_TO_READ = CppErrorCode.FAILED_TO_READ
    FAILED_TO_WRITE = CppErrorCode.FAILED_TO_WRITE
    METHOD_NOT_FOUND = CppErrorCode.METHOD_NOT_FOUND
    INVALID_FUTURE_USE = CppErrorCode.INVALID_FUTURE_USE
    INVALID_CONFIG_VALUE = CppErrorCode.INVALID_CONFIG_VALUE
    CONFIG_PARSE_ERROR = CppErrorCode.CONFIG_PARSE_ERROR
    TIMEOUT = CppErrorCode.TIMEOUT
