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
    MPRPCConfig,
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
    MPRPCConfig,
):
    obj.__module__ = "mprpc.config"
del obj
