"""package of error codes
"""

from .._mprpc_cpp import (
    CommonErrors,
    MessageErrors,
    NetworkErrors,
    ExecutionErrors,
    FutureErrors,
    ConfigErrors,
    ClientErrors,
)

for obj in (
    CommonErrors,
    MessageErrors,
    NetworkErrors,
    ExecutionErrors,
    FutureErrors,
    ConfigErrors,
    ClientErrors,
):
    obj.__module__ = "mprpc.error_code"
del obj

SUCCESS = CommonErrors.SUCCESS
UNEXPECTED_ERROR = CommonErrors.UNEXPECTED_ERROR
UNEXPECTED_NULLPTR = CommonErrors.UNEXPECTED_NULLPTR

PARSE_ERROR = MessageErrors.PARSE_ERROR
INVALID_MESSAGE = MessageErrors.INVALID_MESSAGE

EOF = NetworkErrors.EOF
FAILED_TO_LISTEN = NetworkErrors.FAILED_TO_LISTEN
FAILED_TO_ACCEPT = NetworkErrors.FAILED_TO_ACCEPT
FAILED_TO_RESOLVE = NetworkErrors.FAILED_TO_RESOLVE
FAILED_TO_CONNECT = NetworkErrors.FAILED_TO_CONNECT
FAILED_TO_READ = NetworkErrors.FAILED_TO_READ
FAILED_TO_WRITE = NetworkErrors.FAILED_TO_WRITE

METHOD_NOT_FOUND = ExecutionErrors.METHOD_NOT_FOUND

INVALID_FUTURE_USE = FutureErrors.INVALID_FUTURE_USE

INVALID_CONFIG_VALUE = ConfigErrors.INVALID_CONFIG_VALUE
CONFIG_PARSE_ERROR = ConfigErrors.CONFIG_PARSE_ERROR

TIMEOUT = ClientErrors.TIMEOUT
