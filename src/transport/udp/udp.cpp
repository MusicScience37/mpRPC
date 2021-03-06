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
 * \brief implementation of functions for UDP
 */
#include "mprpc/transport/udp/udp.h"

#include "fmt/core.h"
#include "mprpc/exception.h"
#include "mprpc/logging/labeled_logger.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/require_nonull.h"
#include "mprpc/transport/udp/impl/udp_acceptor.h"
#include "mprpc/transport/udp/impl/udp_connector.h"

namespace mprpc {
namespace transport {
namespace udp {

std::shared_ptr<acceptor> create_udp_acceptor(
    const logging::labeled_logger& logger, thread_pool& threads,
    const std::shared_ptr<compressor_factory>& comp_factory,
    const std::shared_ptr<parser_factory>& parser_factory_ptr,
    const udp_acceptor_config& config) {
    MPRPC_REQUIRE_NONULL(comp_factory);
    MPRPC_REQUIRE_NONULL(parser_factory_ptr);

    const auto& ip_address = config.host.value();
    const auto port = config.port.value();

    asio::error_code err;
    const auto ip_address_parsed = asio::ip::make_address(ip_address, err);
    if (err) {
        MPRPC_ERROR(logger, "invalid IP address: {}", ip_address);
        throw exception(error_info(
            error_code::failed_to_listen, "invalid IP address: " + ip_address));
    }
    const auto endpoint = asio::ip::udp::endpoint(ip_address_parsed, port);
    try {
        auto socket = asio::ip::udp::socket(threads.context(), endpoint);
        const auto acceptor_logger = logging::labeled_logger(
            logger, fmt::format("udp({})", endpoint.port()));
        auto acceptor = std::make_shared<impl::udp_acceptor>(acceptor_logger,
            std::move(socket), threads.context(),
            comp_factory->create_compressor(acceptor_logger),
            parser_factory_ptr->create_parser(acceptor_logger), config);
        MPRPC_INFO(logger, "server started");
        return acceptor;
    } catch (const std::system_error& e) {
        MPRPC_ERROR(
            logger, "failed to listen to {} with {}", endpoint, e.what());
        throw exception(error_info(error_code::failed_to_listen, e.what()));
    }
}

std::shared_ptr<connector> create_udp_connector(
    const logging::labeled_logger& logger, thread_pool& threads,
    const std::shared_ptr<compressor_factory>& comp_factory,
    const std::shared_ptr<parser_factory>& parser_factory_ptr,
    const udp_connector_config& config) {
    MPRPC_REQUIRE_NONULL(comp_factory);
    MPRPC_REQUIRE_NONULL(parser_factory_ptr);

    const auto& host = config.host.value();
    const auto port = config.port.value();

    asio::ip::udp::resolver resolver(threads.context());
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
    const auto endpoint = resolved_endpoints.begin()->endpoint();

    auto socket = asio::ip::udp::socket(threads.context());
    if (endpoint.address().is_v4()) {
        socket = asio::ip::udp::socket(
            threads.context(), asio::ip::udp::endpoint(asio::ip::udp::v4(), 0));
    } else {
        socket = asio::ip::udp::socket(
            threads.context(), asio::ip::udp::endpoint(asio::ip::udp::v6(), 0));
    }

    MPRPC_DEBUG(logger, "send UDP packets from {} to {}",
        socket.local_endpoint(), endpoint);

    const auto connector_logger = logging::labeled_logger(
        logger, fmt::format("udp({})", socket.local_endpoint().port()));
    return std::make_shared<impl::udp_connector>(connector_logger,
        std::move(socket), endpoint, threads.context(),
        comp_factory->create_compressor(connector_logger),
        parser_factory_ptr->create_parser(connector_logger), config);
}

}  // namespace udp
}  // namespace transport
}  // namespace mprpc
