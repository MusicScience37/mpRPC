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
    ServerConfig,
    TransportType,
    ClientConfig,
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


def test_server_config():
    """test of ServerConfig class
    """

    config = ServerConfig()

    config.num_threads = 1

    tcp = TCPAcceptorConfig()
    config.tcp_acceptors = [tcp]

    udp = UDPAcceptorConfig()
    config.udp_acceptors = [udp]

    with pytest.raises(MPRPCException):
        config.num_threads = 0


def test_client_config():
    """test of ClientConfig class
    """

    config = ClientConfig()

    config.num_threads = 1
    config.sync_request_timeout_ms = 10000
    config.connector_type = TransportType.UDP
    config.tcp_connector.compression.type = CompressionType.ZSTD
    config.udp_connector.compression.type = CompressionType.ZSTD
