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
 * \brief declaration and implementation of tcp_acceptor class
 */
#pragma once

#include <functional>
#include <memory>

#include <asio/ip/tcp.hpp>

#include "mprpc/error_info.h"
#include "mprpc/exception.h"
#include "mprpc/logging/logger.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/thread_pool.h"
#include "mprpc/transport/acceptor.h"
#include "mprpc/transport/parser.h"
#include "mprpc/transport/tcp/tcp_address.h"
#include "mprpc/transport/tcp/tcp_session.h"

namespace mprpc {
namespace transport {
namespace tcp {

/*!
 * \brief class of acceptors on TCP sockets
 */
class tcp_acceptor : public acceptor {
public:
    /*!
     * \brief construct
     *
     * \param logger logger
     * \param endpoint server endpoint
     * \param io_context io_context
     * \param parser_factory_ptr factory of parsers
     */
    tcp_acceptor(std::shared_ptr<mprpc::logging::logger> logger,
        const asio::ip::tcp::endpoint& endpoint, asio::io_context& io_context,
        std::shared_ptr<parser_factory> parser_factory_ptr)
        : logger_(std::move(logger)),
          socket_(io_context, endpoint),
          io_context_(io_context),
          parser_factory_(std::move(parser_factory_ptr)) {
        MPRPC_INFO(logger_, "server started");
    }

    //! \copydoc mprpc::transport::acceptor::async_accept
    void async_accept(on_accept_handler_type handler) override {
        socket_.async_accept([this, moved_handler = std::move(handler)](
                                 const asio::error_code& error,
                                 asio::ip::tcp::socket socket) mutable {
            this->on_accept(error, std::move(socket), moved_handler);
        });
    }

    /*!
     * \brief process on a connection accepted
     *
     * \param error error
     * \param socket socket
     * \param handler handler
     */
    void on_accept(const asio::error_code& error, asio::ip::tcp::socket socket,
        const on_accept_handler_type& handler) {
        if (error) {
            if (error == asio::error::operation_aborted) {
                return;
            }
            MPRPC_ERROR(logger_, error.message());
            handler(error_info(error_code::failed_to_read, error.message()),
                nullptr);
            return;
        }

        handler(error_info(),
            std::make_shared<tcp_session>(logger_, std::move(socket),
                io_context_,
                parser_factory_->create_streaming_parser(logger_)));
    }

    //! \copydoc mprpc::transport::acceptor::local_address
    std::shared_ptr<const address> local_address() const override {
        return std::make_shared<tcp_address>(socket_.local_endpoint());
    }

private:
    //! logger
    std::shared_ptr<mprpc::logging::logger> logger_;

    //! socket
    asio::ip::tcp::acceptor socket_;

    //! io_context
    asio::io_context& io_context_;

    //! factory of parsers
    std::shared_ptr<parser_factory> parser_factory_;
};

}  // namespace tcp
}  // namespace transport
}  // namespace mprpc