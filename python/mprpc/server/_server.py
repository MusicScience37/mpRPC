"""implementation of Server class
"""

from mprpc.logging import Logger
from mprpc.config import ServerConfig
from mprpc.execution import MethodServer
from .._mprpc_cpp import PythonServerHelper


class Server:
    """class of server

    Parameters
    ----------
    logger : Logger
        logger
    config : ServerConfig
        server configuration
    """

    def __init__(self, logger: Logger, config: ServerConfig, method_server: MethodServer):
        self.__impl = PythonServerHelper(
            logger, config, method_server.async_process_message)

    def start(self):
        """start process
        """

        self.__impl.start()

    def stop(self):
        """stop process
        """

        self.__impl.stop()
