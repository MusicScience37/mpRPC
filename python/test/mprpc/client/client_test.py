"""test of Client class
"""

from threading import Event

from mprpc.execution import FunctionMethodExecutor, SimpleMethodServer
from mprpc.server import Server
from mprpc.client import Client
from mprpc.logging import LogLevel, PythonLogger
from mprpc.config import ServerConfig, TCPAcceptorConfig, ClientConfig


ADD_CALLED = Event()


def add(left: int, right: int) -> int:
    """method function for test
    """

    ADD_CALLED.set()
    return left + right


def test_client():
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
    client = Client(logger, client_config)
    client.start()

    assert client.request(name, 2, 3) == 5
    assert ADD_CALLED.is_set()

    ADD_CALLED.clear()
    client.notify(name, 2, 3)
    ADD_CALLED.wait(timeout=10)

    client.stop()
    server.stop()
