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
 * \brief declaration and implementation of tcp_connector class
 */
#pragma once

#include <asio/ip/tcp.hpp>

#include "mprpc/logging/logger.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/thread_pool.h"
#include "mprpc/transport/asio_helper/basic_endpoint.h"
#include "mprpc/transport/asio_helper/stream_socket_helper.h"
#include "mprpc/transport/connector.h"
#include "mprpc/transport/parser.h"
#include "mprpc/transport/tcp/tcp_address.h"

namespace mprpc {
namespace transport {
namespace tcp {

/*!
 * \brief class of connectors on TCP sockets
 */
class tcp_connector : public connector {
public:
    /*!
     * \brief construct
     *
     * \param logger logger
     * \param socket socket
     * \param threads thread pool
     * \param parser parser
     */
    tcp_connector(const std::shared_ptr<logging::logger>& logger,
        asio::ip::tcp::socket socket,
        const std::shared_ptr<thread_pool>& threads,
        std::shared_ptr<streaming_parser> parser)
        : socket_helper_(
              logger, std::move(socket), threads, std::move(parser)) {
        MPRPC_INFO(logger, "conencted {} to {}",
            socket_helper_.socket().local_endpoint(),
            socket_helper_.socket().remote_endpoint());
    }

    //! \copydoc mprpc::transport::connector::async_read
    void async_read(on_read_handler_type handler) override {
        socket_helper_.async_read(std::move(handler));
    }

    //! \copydoc mprpc::transport::connector::async_write
    void async_write(
        const message_data& data, on_write_handler_type handler) override {
        socket_helper_.async_write(data, std::move(handler));
    }

    //! \copydoc mprpc::transport::connector::local_address
    std::shared_ptr<const address> local_address() const override {
        return std::make_shared<tcp_address>(
            socket_helper_.socket().local_endpoint());
    }

    //! \copydoc mprpc::transport::connector::remote_address
    std::shared_ptr<const address> remote_address() const override {
        return std::make_shared<tcp_address>(
            socket_helper_.socket().remote_endpoint());
    }

private:
    //! socket helper
    asio_helper::stream_socket_helper<asio::ip::tcp::socket> socket_helper_;
};

}  // namespace tcp
}  // namespace transport
}  // namespace mprpc
