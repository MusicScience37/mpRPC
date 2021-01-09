"""test of SimpleMethodServer class
"""

from threading import Event

from mprpc_cpp_test import MockSession
from mprpc.message import (
    MsgType, Request, Notification, validate_message, unpack_object, MessageData,
)
from mprpc.execution import SimpleMethodServer, FunctionMethodExecutor
from mprpc import ErrorInfo, ErrorCode


def add(left: int, right: int) -> int:
    """method function for test
    """

    return left + right


def test_simple_method_server_request():
    """test of requests to SimpleMethodServer class
    """

    name = 'test_method'
    executor = FunctionMethodExecutor(name, add)
    server = SimpleMethodServer([executor])

    session = MockSession()

    msgid = 37
    params = [2, 3]
    request = Request(MsgType.REQUEST, msgid, name, params)

    err = None
    has_response = None
    response = None
    processed = Event()

    def handler(err_in: ErrorInfo, has_response_in: bool, response_in: MessageData):
        """handler
        """

        nonlocal err
        nonlocal has_response
        nonlocal response
        nonlocal processed

        err = err_in
        has_response = has_response_in
        response = response_in
        processed.set()

    server.async_process_message(session, request, handler)

    timeout = 10.0
    processed.wait(timeout=timeout)
    assert processed.is_set()

    assert err is not None
    assert not err
    assert has_response
    assert response is not None

    response = validate_message(unpack_object(response))

    assert response.msgtype == MsgType.RESPONSE
    assert response.msgid == msgid
    assert response.error is None
    assert response.result == 5


def test_simple_method_server_request_invalid_method():
    """test of requests for non-existing methods to SimpleMethodServer class
    """

    name = 'test_method'
    executor = FunctionMethodExecutor(name, add)
    server = SimpleMethodServer([executor])

    session = MockSession()

    msgid = 37
    name = 'invalid_method'
    params = [2, 3]
    request = Request(MsgType.REQUEST, msgid, name, params)

    err = None
    has_response = None
    response = None
    processed = Event()

    def handler(err_in: ErrorInfo, has_response_in: bool, response_in: MessageData):
        """handler
        """

        nonlocal err
        nonlocal has_response
        nonlocal response
        nonlocal processed

        err = err_in
        has_response = has_response_in
        response = response_in
        processed.set()

    server.async_process_message(session, request, handler)

    timeout = 10.0
    processed.wait(timeout=timeout)
    assert processed.is_set()

    assert err is not None
    assert not err
    assert has_response
    assert response is not None

    response = validate_message(unpack_object(response))

    assert response.msgtype == MsgType.RESPONSE
    assert response.msgid == msgid
    assert response.error is not None

def test_simple_method_server_notification():
    """test of notifications to SimpleMethodServer class
    """

    name = 'test_method'
    executor = FunctionMethodExecutor(name, add)
    server = SimpleMethodServer([executor])

    session = MockSession()

    params = [2, 3]
    notification = Notification(MsgType.NOTIFICATION, name, params)

    err = None
    has_response = None
    response = None
    processed = Event()

    def handler(err_in: ErrorInfo, has_response_in: bool, response_in: MessageData):
        """handler
        """

        nonlocal err
        nonlocal has_response
        nonlocal response
        nonlocal processed

        err = err_in
        has_response = has_response_in
        response = response_in
        processed.set()

    server.async_process_message(session, notification, handler)

    timeout = 10.0
    processed.wait(timeout=timeout)
    assert processed.is_set()

    assert err is not None
    assert not err
    assert has_response is not None
    assert not has_response
    assert response == MessageData()
