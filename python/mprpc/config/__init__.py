"""module of configuration
"""

from .._mprpc_cpp import (
    CompressionType,
    CompressionConfig,
    TCPAcceptorConfig,
    TCPConnectorConfig,
)

for obj in (
    CompressionType,
    CompressionConfig,
    TCPAcceptorConfig,
    TCPConnectorConfig,
):
    obj.__module__ = "mprpc.config"
del obj
