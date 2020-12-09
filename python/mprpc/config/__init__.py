"""module of configuration
"""

from .._mprpc_cpp import (
    CompressionType,
    CompressionConfig,
    TCPAcceptorConfig,
    TCPConnectorConfig,
    UDPAcceptorConfig,
    UDPConnectorConfig,
    ServerConfig,
    TransportType,
    ClientConfig,
)

for obj in (
    CompressionType,
    CompressionConfig,
    TCPAcceptorConfig,
    TCPConnectorConfig,
    UDPAcceptorConfig,
    UDPConnectorConfig,
    ServerConfig,
    TransportType,
    ClientConfig,
):
    obj.__module__ = "mprpc.config"
del obj
