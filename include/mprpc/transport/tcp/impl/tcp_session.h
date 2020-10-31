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
 * \brief declaration and implementation of tcp_session class
 */
#pragma once

#include <future>

#include <asio/ip/tcp.hpp>

#include "mprpc/logging/logger.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/thread_pool.h"
#include "mprpc/transport/asio_helper/basic_endpoint.h"
#include "mprpc/transport/asio_helper/stream_socket_helper.h"
#include "mprpc/transport/parser.h"
#include "mprpc/transport/session.h"
#include "mprpc/transport/tcp/impl/tcp_address.h"
#include "mprpc/transport/tcp/tcp_acceptor_config.h"

namespace mprpc {
namespace transport {
namespace tcp {

/*!
 * \brief class of sessions on TCP sockets
 */
class tcp_session : public session {
public:
    /*!
     * \brief construct
     *
     * \param logger logger
     * \param socket socket
     * \param io_context io_context
     * \param parser parser
     * \param config configuration
     */
    tcp_session(const std::shared_ptr<logging::logger>& logger,
        asio::ip::tcp::socket socket, asio::io_context& io_context,
        std::shared_ptr<streaming_parser> parser,
        const tcp_acceptor_config& config)
        : socket_helper_(std::make_shared<socket_helper_type>(logger,
              std::move(socket), io_context, std::move(parser), config)) {
        MPRPC_INFO(logger, "accepted connection from {} at {}",
            socket_helper_->socket().remote_endpoint(),
            socket_helper_->socket().local_endpoint());
    }

    //! \copydoc mprpc::transport::session::async_read
    void async_read(on_read_handler_type handler) override {
        socket_helper_->async_read(std::move(handler));
    }

    //! \copydoc mprpc::transport::session::async_write
    void async_write(
        const message_data& data, on_write_handler_type handler) override {
        socket_helper_->async_write(data, std::move(handler));
    }

    //! \copydoc mprpc::transport::session::remote_address
    std::shared_ptr<const address> remote_address() const override {
        return std::make_shared<tcp_address>(
            socket_helper_->socket().remote_endpoint());
    }

private:
    //! socket helper type
    using socket_helper_type =
        asio_helper::stream_socket_helper<asio::ip::tcp::socket>;

    //! socket helper
    std::shared_ptr<socket_helper_type> socket_helper_;
};

}  // namespace tcp
}  // namespace transport
}  // namespace mprpc
