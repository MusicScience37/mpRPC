"""test of Server class
"""

from threading import Event

from mprpc.execution import FunctionMethodExecutor, SimpleMethodServer
from mprpc.server import Server
from mprpc.client import PythonClientHelper
from mprpc.logging import LogLevel, PythonLogger
from mprpc.config import ServerConfig, TCPAcceptorConfig, ClientConfig
from mprpc import ErrorInfo
from mprpc.message import pack_request, MessageData, unpack_object, validate_message, MsgType


def add(left: int, right: int) -> int:
    """method function for test
    """

    return left + right


def test_server():
    """test of Server class
    """

    logger = PythonLogger(LogLevel.TRACE)

    name = 'add'
    logger = PythonLogger()
    executor = FunctionMethodExecutor(logger, name, add)
    method_server = SimpleMethodServer(logger, [executor])

    server_config = ServerConfig()
    server_config.tcp_acceptors = [TCPAcceptorConfig()]
    server = Server(logger, server_config, method_server)
    server.start()

    client_config = ClientConfig()

    response_error = None
    response_data = None
    response_received_event = Event()

    def client_process_message(error: ErrorInfo, data: MessageData):
        nonlocal response_error
        nonlocal response_data
        nonlocal response_received_event
        response_error = error
        response_data = data
        response_received_event.set()

    client = PythonClientHelper(logger, client_config, client_process_message)
    client.start()

    request_error = None
    request_sent_event = Event()

    def on_request_sent(error: ErrorInfo):
        nonlocal request_error
        nonlocal request_sent_event
        request_error = error
        request_sent_event.set()

    msgid = 37
    data = pack_request(msgid, name, [2, 3])
    client.async_send(data, on_request_sent)

    timeout = 10.0
    request_sent_event.wait(timeout=timeout)
    assert request_sent_event.is_set()
    assert not request_error

    response_received_event.wait(timeout=timeout)
    assert response_received_event.is_set()
    assert not response_error

    response = validate_message(unpack_object(response_data))
    assert response.msgtype == MsgType.RESPONSE
    assert response.msgid == msgid
    assert response.error is None
    assert response.result == 5

    client.stop()
    server.stop()
