/*
 * Copyright 2020 MusicScience37 (Kenta Kabashima)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*!
 * \file
 * \brief implementation of bind_config function
 */
#include "mprpc/bind_config.h"

#include <pybind11/stl.h>

#include "mprpc/client_config.h"
#include "mprpc/config/load.h"
#include "mprpc/mprpc_config.h"
#include "mprpc/server_config.h"
#include "mprpc/transport/compression_config.h"
#include "mprpc/transport/tcp/tcp_acceptor_config.h"
#include "mprpc/transport/tcp/tcp_connector_config.h"
#include "mprpc/transport/udp/udp_acceptor_config.h"
#include "mprpc/transport/udp/udp_connector_config.h"

namespace {

template <typename S1, typename S2, typename C>
void bind_value(pybind11::class_<S1>& s, C S2::*c) {
    static auto name = C::option_type::name();
    s.def_property(
        name.c_str(), [c](S1& self) { return (self.*c).value(); },
        [c](S1& self, const typename C::value_type& val) { (self.*c) = val; });
}

}  // namespace

namespace mprpc {
namespace python {

void bind_config(pybind11::module& module) {
    using mprpc::transport::compression_type;
    pybind11::enum_<compression_type>(
        module, "CompressionType", "enumeration of compression types")
        .value("NONE", compression_type::none, "no compression")
        .value("ZSTD", compression_type::zstd, "compression with zstd library");

    using mprpc::transport::compression_config;
    auto compression_config_class =
        pybind11::class_<compression_config>(module, "CompressionConfig",
            R"doc(CompressionConfig()

            configuration for compression

            Attributes
            ----------
            type : mprpc.config.CompressionType
                compression type (none, zstd)
            zstd_compression_level : int
                compression levels in zstd library
        )doc");
    compression_config_class.def(pybind11::init<>());
    bind_value(compression_config_class, &compression_config::type);
    bind_value(
        compression_config_class, &compression_config::zstd_compression_level);

    using mprpc::transport::tcp::tcp_acceptor_config;
    auto tcp_acceptor_config_class = pybind11::class_<tcp_acceptor_config>(
        module, "TCPAcceptorConfig", R"doc(TCPAcceptorConfig()

            configuration of TCP acceptor

            Attributes
            ----------
            compression : mprpc.config.CompressionConfig
                configuration of compression
            host : str
                host address (IP address for server, common names for clients)
            port : int
                port number
            streaming_min_buf_size : int
                minimum buffer size in streaming parser [bytes]
        )doc");
    tcp_acceptor_config_class.def(pybind11::init<>());
    tcp_acceptor_config_class.def_readwrite(
        "compression", &tcp_acceptor_config::compression);
    bind_value(tcp_acceptor_config_class, &tcp_acceptor_config::host);
    bind_value(tcp_acceptor_config_class, &tcp_acceptor_config::port);
    bind_value(tcp_acceptor_config_class,
        &tcp_acceptor_config::streaming_min_buf_size);

    using mprpc::transport::tcp::tcp_connector_config;
    auto tcp_connector_config_class = pybind11::class_<tcp_connector_config>(
        module, "TCPConnectorConfig", R"doc(TCPConnectorConfig()

            configuration of TCP connector

            Attributes
            ----------
            compression : mprpc.config.CompressionConfig
                configuration of compression
            host : str
                host address (IP address for server, common names for clients)
            port : int
                port number
            streaming_min_buf_size : int
                minimum buffer size in streaming parser [bytes]
        )doc");
    tcp_connector_config_class.def(pybind11::init<>());
    tcp_connector_config_class.def_readwrite(
        "compression", &tcp_connector_config::compression);
    bind_value(tcp_connector_config_class, &tcp_connector_config::host);
    bind_value(tcp_connector_config_class, &tcp_connector_config::port);
    bind_value(tcp_connector_config_class,
        &tcp_connector_config::streaming_min_buf_size);

    using mprpc::transport::udp::udp_acceptor_config;
    auto udp_acceptor_config_class = pybind11::class_<udp_acceptor_config>(
        module, "UDPAcceptorConfig", R"doc(UDPAcceptorConfig()

            configuration for UDP acceptors

            Attributes
            ----------
            compression : mprpc.config.CompressionConfig
                configuration of compression
            host : str
                host address (IP address for server, common names for clients)
            port : int
                port number
            datagram_buf_size : int
                buffer size in datagrams
        )doc");
    udp_acceptor_config_class.def(pybind11::init<>());
    udp_acceptor_config_class.def_readwrite(
        "compression", &udp_acceptor_config::compression);
    bind_value(udp_acceptor_config_class, &udp_acceptor_config::host);
    bind_value(udp_acceptor_config_class, &udp_acceptor_config::port);
    bind_value(
        udp_acceptor_config_class, &udp_acceptor_config::datagram_buf_size);

    using mprpc::transport::udp::udp_connector_config;
    auto udp_connector_config_class = pybind11::class_<udp_connector_config>(
        module, "UDPConnectorConfig", R"doc(UDPConnectorConfig()

            configuration for UDP acceptors

            Attributes
            ----------
            compression : mprpc.config.CompressionConfig
                configuration of compression
            host : str
                host address (IP address for server, common names for clients)
            port : int
                port number
            datagram_buf_size : int
                buffer size in datagrams
        )doc");
    udp_connector_config_class.def(pybind11::init<>());
    udp_connector_config_class.def_readwrite(
        "compression", &udp_connector_config::compression);
    bind_value(udp_connector_config_class, &udp_connector_config::host);
    bind_value(udp_connector_config_class, &udp_connector_config::port);
    bind_value(
        udp_connector_config_class, &udp_connector_config::datagram_buf_size);

    using mprpc::server_config;
    auto server_config_class = pybind11::class_<server_config>(
        module, "ServerConfig", R"doc(ServerConfig()

            configuration of servers

            Attributes
            ----------
            num_threads : int
                number of threads
            tcp_acceptors : List[mprpc.config.TCPAcceptorConfig]
                TCP acceptors
            udp_acceptors : List[mprpc.config.UDPAcceptorConfig]
                UDP acceptors
        )doc");
    server_config_class.def(pybind11::init<>());
    bind_value(server_config_class, &server_config::num_threads);
    server_config_class.def_readwrite(
        "tcp_acceptors", &server_config::tcp_acceptors);
    server_config_class.def_readwrite(
        "udp_acceptors", &server_config::udp_acceptors);

    using mprpc::transport_type;
    pybind11::enum_<transport_type>(
        module, "TransportType", "enumeration of transport types")
        .value("TCP", transport_type::tcp, "TCP")
        .value("UDP", transport_type::udp, "UDP");

    using mprpc::client_config;
    auto client_config_class = pybind11::class_<client_config>(
        module, "ClientConfig", R"doc(ClientConfig()

            configuration of clients

            Attributes
            ----------
            num_threads : int
                number of threads
            sync_request_timeout_ms : int
                timeout of synchronous requests [ms]
            connector_type : mprpc.config.TransportType
                connector type (TCP, UDP)
            tcp_connector : mprpc.config.TCPConnectorConfig
                TCP connector configuration
            udp_connector : mprpc.config.UDPConnectorConfig
                UDP connector configuration
        )doc");
    client_config_class.def(pybind11::init<>());
    bind_value(client_config_class, &client_config::num_threads);
    bind_value(client_config_class, &client_config::sync_request_timeout_ms);
    bind_value(client_config_class, &client_config::connector_type);
    client_config_class.def_readwrite(
        "tcp_connector", &client_config::tcp_connector);
    client_config_class.def_readwrite(
        "udp_connector", &client_config::udp_connector);

    using mprpc::mprpc_config;
    pybind11::class_<mprpc_config>(module, "MPRPCConfig", R"doc(MPRPCConfig()

            configuration of this library

            Attributes
            ----------
            client : mprpc.config.ClientConfig
                configuration of clients
            server : mprpc.config.ServerConfig
                configuration of servers
        )doc")
        .def(pybind11::init<>())
        .def_readwrite("client", &mprpc_config::client)
        .def_readwrite("server", &mprpc_config::server);

    using mprpc::config::load;
    module.def("load", &load,
        R"doc(load(config_filepath : str) -> mprpc.config.MPRPCConfig

            load configuration file

            Parameters
            ----------
            config_filepath : str
                configuration file path

            Returns
            -------
            mprpc.config.MPRPCConfig
                configuration
        )doc");
}

}  // namespace python
}  // namespace mprpc
