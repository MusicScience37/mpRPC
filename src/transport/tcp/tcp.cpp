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

#include "mprpc/exception.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/transport/tcp/impl/tcp_acceptor.h"
#include "mprpc/transport/tcp/impl/tcp_connector.h"

namespace mprpc {
namespace transport {
namespace tcp {

std::shared_ptr<acceptor> create_tcp_acceptor(
    const std::shared_ptr<mprpc::logging::logger>& logger,
    const std::string& host, const std::uint16_t& port, thread_pool& threads,
    const std::shared_ptr<parser_factory>& parser_factory_ptr) {
    try {
        const auto ip_address = asio::ip::make_address(host);
        const auto endpoint = asio::ip::tcp::endpoint(ip_address, port);
        return std::make_unique<tcp_acceptor>(
            logger, endpoint, threads.context(), parser_factory_ptr);
    } catch (const asio::system_error& e) {
        MPRPC_ERROR(
            logger, "failed to listen to {}:{} with {}", host, port, e.what());
        throw exception(error_info(error_code::failed_to_listen, e.what()));
    }
}

std::shared_ptr<connector> create_tcp_connector(
    const std::shared_ptr<mprpc::logging::logger>& logger,
    const std::string& host, const std::uint16_t& port, thread_pool& threads,
    const std::shared_ptr<parser_factory>& parser_factory_ptr) {
    try {
        const auto ip_address = asio::ip::make_address(host);
        const auto endpoint = asio::ip::tcp::endpoint(ip_address, port);
        auto connector_socket = asio::ip::tcp::socket(threads.context());
        connector_socket.connect(endpoint);
        return std::make_unique<tcp_connector>(logger,
            std::move(connector_socket), threads.context(),
            parser_factory_ptr->create_streaming_parser(logger));
    } catch (const asio::system_error& e) {
        MPRPC_ERROR(
            logger, "failed to connect to {}:{} with {}", host, port, e.what());
        throw exception(error_info(error_code::failed_to_listen, e.what()));
    }
}

}  // namespace tcp
}  // namespace transport
}  // namespace mprpc
