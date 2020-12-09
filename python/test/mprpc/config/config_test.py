"""test of configuration
"""

import pytest

from mprpc.config import (
    CompressionType,
    CompressionConfig,
    TCPAcceptorConfig,
    TCPConnectorConfig,
    UDPAcceptorConfig,
    UDPConnectorConfig,
)
from mprpc import MPRPCException


def test_compression_config():
    """test of CompressionConfig class
    """

    config = CompressionConfig()

    config.type = CompressionType.ZSTD
    config.zstd_compression_level = 0
    with pytest.raises(MPRPCException):
        config.zstd_compression_level = 100000


def test_tcp_acceptor_config():
    """test of TCPAcceptorConfig class
    """

    config = TCPAcceptorConfig()

    config.compression.type = CompressionType.ZSTD
    config.host = '127.0.0.1'
    config.port = 12345
    config.streaming_min_buf_size = 1000


def test_tcp_connector_config():
    """test of TCPConnectorConfig class
    """

    config = TCPConnectorConfig()

    config.compression.type = CompressionType.ZSTD
    config.host = '127.0.0.1'
    config.port = 12345
    config.streaming_min_buf_size = 1000


def test_udp_acceptor_config():
    """test of UDPAcceptorConfig class
    """

    config = UDPAcceptorConfig()

    config.compression.type = CompressionType.ZSTD
    config.host = '127.0.0.1'
    config.port = 12345
    config.datagram_buf_size = 1000


def test_udp_connector_config():
    """test of UDPConnectorConfig class
    """

    config = UDPConnectorConfig()

    config.compression.type = CompressionType.ZSTD
    config.host = '127.0.0.1'
    config.port = 12345
    config.datagram_buf_size = 1000
