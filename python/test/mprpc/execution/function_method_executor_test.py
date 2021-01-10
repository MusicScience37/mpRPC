"""test of FunctionMethodExecutor class
"""

from mprpc_cpp_test import MockSession
from mprpc.logging import PythonLogger
from mprpc.message import MsgType, Request, Notification, validate_message, unpack_object
from mprpc.execution import FunctionMethodExecutor


def add(left: int, right: int) -> int:
    """method function for test
    """

    return left + right


def test_function_method_executor_request():
    """test of requests in FunctionMethodExecutor class
    """

    name = 'test_method'

    executor = FunctionMethodExecutor(PythonLogger(), name, add)

    assert executor.name == name
    assert executor.function == add  # pylint: disable=comparison-with-callable

    session = MockSession()

    msgid = 37
    params = [2, 3]
    request = Request(MsgType.REQUEST, msgid, name, params)

    response_data = executor.process_request(session, request)
    response = validate_message(unpack_object(response_data))

    assert response.msgtype == MsgType.RESPONSE
    assert response.msgid == msgid
    assert response.error is None
    assert response.result == 5

    msgid = 37
    params = [2]
    request = Request(MsgType.REQUEST, msgid, name, params)

    response_data = executor.process_request(session, request)
    response = validate_message(unpack_object(response_data))

    assert response.msgtype == MsgType.RESPONSE
    assert response.msgid == msgid
    assert response.error is not None


def test_function_method_executor_notification():
    """test of notifications in FunctionMethodExecutor class
    """

    name = 'test_method'

    executor = FunctionMethodExecutor(PythonLogger(), name, add)

    assert executor.name == name
    assert executor.function == add  # pylint: disable=comparison-with-callable

    session = MockSession()

    params = [2, 3]
    notification = Notification(MsgType.NOTIFICATION, name, params)

    session = MockSession()

    executor.process_notification(session, notification)

    params = [2]
    notification = Notification(MsgType.NOTIFICATION, name, params)

    executor.process_notification(session, notification)
