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
 * \brief implementation of TCP-related factory functions
 */
#include "mprpc/transport/tcp/tcp.h"

#include <asio/ip/address.hpp>
#include <asio/ip/tcp.hpp>

#include "mprpc/exception.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/transport/asio_helper/basic_endpoint.h"
#include "mprpc/transport/tcp/impl/tcp_acceptor.h"
#include "mprpc/transport/tcp/impl/tcp_connector.h"

namespace mprpc {
namespace transport {
namespace tcp {

std::shared_ptr<acceptor> create_tcp_acceptor(logging::labeled_logger logger,
    thread_pool& threads,
    const std::shared_ptr<compressor_factory>& comp_factory,
    const std::shared_ptr<parser_factory>& parser_factory_ptr,
    const tcp_acceptor_config& config) {
    const auto& ip_address = config.host.value();
    const auto port = config.port.value();
    asio::error_code err;
    const auto ip_address_parsed = asio::ip::make_address(ip_address, err);
    if (err) {
        MPRPC_ERROR(logger, "invalid IP address: {}", ip_address);
        throw exception(error_info(
            error_code::failed_to_listen, "invalid IP address: " + ip_address));
    }
    const auto endpoint = asio::ip::tcp::endpoint(ip_address_parsed, port);
    return std::make_unique<impl::tcp_acceptor>(logger, endpoint,
        threads.context(), comp_factory, parser_factory_ptr, config);
}

std::shared_ptr<connector> create_tcp_connector(logging::labeled_logger logger,
    thread_pool& threads,
    const std::shared_ptr<compressor_factory>& comp_factory,
    const std::shared_ptr<parser_factory>& parser_factory_ptr,
    const tcp_connector_config& config) {
    const auto& host = config.host.value();
    const auto port = config.port.value();
    asio::ip::tcp::resolver resolver{threads.context()};
    asio::error_code err;
    MPRPC_DEBUG(logger, "resloving {}:{}", host, port);
    auto resolved_endpoints = resolver.resolve(host, std::to_string(port), err);
    if (err) {
        MPRPC_ERROR(logger, "failed to resolve {}: {}", host, err.message());
        throw exception(error_info(error_code::failed_to_resolve,
            "failed to resolve " + host + ": " + err.message()));
    }

    for (const auto& entry : resolved_endpoints) {
        MPRPC_TRACE(logger, "resolved endpoint: {}", entry.endpoint());
    }

    auto connector_socket = asio::ip::tcp::socket(threads.context());
    for (const auto& entry : resolved_endpoints) {
        MPRPC_DEBUG(
            logger, "trying to connect to endpoint: {}", entry.endpoint());
        connector_socket.connect(entry.endpoint(), err);
        if (!err) {
            return std::make_unique<impl::tcp_connector>(logger,
                std::move(connector_socket), threads.context(),
                comp_factory->create_streaming_compressor(logger),
                parser_factory_ptr->create_streaming_parser(logger), config);
        }

        MPRPC_DEBUG(logger, "failed to connect to {}: {}", entry.endpoint(),
            err.message());
    }
    MPRPC_ERROR(
        logger, "failed to connect to {}:{}: {}", host, port, err.message());
    throw exception(error_info(error_code::failed_to_connect,
        "failed to connect to " + host + ":" + std::to_string(port) + ": " +
            err.message()));
}

}  // namespace tcp
}  // namespace transport
}  // namespace mprpc
