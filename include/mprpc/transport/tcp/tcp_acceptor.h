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
    //! type of factories of parsers
    using parser_factory_type =
        std::function<std::shared_ptr<streaming_parser>()>;

    /*!
     * \brief construct
     *
     * \param logger logger
     * \param endpoint server endpoint
     * \param threads thread pool
     * \param parser_factory factory of parsers
     */
    tcp_acceptor(std::shared_ptr<mprpc::logging::logger> logger,
        const asio::ip::tcp::endpoint& endpoint,
        std::shared_ptr<mprpc::thread_pool> threads,
        parser_factory_type parser_factory)
        : logger_(std::move(logger)),
          socket_(threads->context(), endpoint),
          threads_(std::move(threads)),
          parser_factory_(std::move(parser_factory)) {
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
            std::make_shared<tcp_session>(
                logger_, std::move(socket), threads_, parser_factory_()));
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

    //! thread pool
    std::shared_ptr<mprpc::thread_pool> threads_;

    //! factory of parsers
    parser_factory_type parser_factory_;
};

}  // namespace tcp
}  // namespace transport
}  // namespace mprpc
