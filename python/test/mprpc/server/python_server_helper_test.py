"""test of PythonServerHelper class
"""

from threading import Event
from typing import Callable, Any

from mprpc.server import PythonServerHelper
from mprpc.client import PythonClientHelper
from mprpc.logging import LogLevel, PythonLogger
from mprpc.config import ServerConfig, TCPAcceptorConfig, ClientConfig
from mprpc import ErrorInfo
from mprpc.message import pack_object, MessageData


def test_python_server_helper():
    """test of PythonServerHelper class
    """

    logger = PythonLogger(LogLevel.TRACE)

    request_received_event = Event()

    def server_process_message(_, data: MessageData,
                               handler: Callable[[ErrorInfo, bool, MessageData], Any]):
        nonlocal request_received_event
        request_received_event.set()
        handler(ErrorInfo(), True, data)

    server_config = ServerConfig()
    server_config.tcp_acceptors = [TCPAcceptorConfig()]
    server = PythonServerHelper(logger, server_config, server_process_message)
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

    data = pack_object([1, 2, 3])
    client.async_send(data, on_request_sent)

    timeout = 10.0
    request_sent_event.wait(timeout=timeout)
    assert request_sent_event.is_set()
    assert not request_error

    request_received_event.wait(timeout=timeout)
    assert request_received_event.is_set()

    response_received_event.wait(timeout=timeout)
    assert response_received_event.is_set()
    assert not response_error
    assert response_data == data

    client.stop()
    server.stop()
