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
 * \brief declaration and implementation of udp_acceptor class
 */
#pragma once

#include <memory>

#include "mprpc/logging/logger.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/thread_pool.h"
#include "mprpc/transport/acceptor.h"
#include "mprpc/transport/asio_helper/basic_endpoint.h"
#include "mprpc/transport/parser.h"
#include "mprpc/transport/udp/impl/udp_address.h"
#include "mprpc/transport/udp/impl/udp_common.h"
#include "mprpc/transport/udp/impl/udp_session.h"
#include "mprpc/transport/udp/udp_acceptor_config.h"

namespace mprpc {
namespace transport {
namespace udp {
namespace impl {

/*!
 * \brief class of acceptors on UDP socket
 */
class udp_acceptor final : public acceptor {
public:
    /*!
     * \brief construct
     *
     * \param logger logger
     * \param socket socket
     * \param io_context io_context
     * \param comp compressor
     * \param parser_ptr parser
     * \param config configuration
     */
    udp_acceptor(const std::shared_ptr<mprpc::logging::logger>& logger,
        asio::ip::udp::socket socket, asio::io_context& io_context,
        std::shared_ptr<compressor> comp, std::shared_ptr<parser> parser_ptr,
        udp_acceptor_config config)
        : logger_(logger),
          helper_(std::make_shared<udp_common>(logger, std::move(socket),
              io_context, std::move(comp), std::move(parser_ptr), config_)),
          config_(config) {
        helper_->init();
    }

    //! \copydoc mprpc::transport::acceptor::async_accept
    void async_accept(on_accept_handler_type handler) override {
        helper_->async_read([this, handler](const error_info& error,
                                const asio::ip::udp::endpoint& endpoint,
                                const message_data& data) {
            this->on_read(error, endpoint, data, handler);
        });
    }

    //! \copydoc mprpc::transport::acceptor::local_address
    std::shared_ptr<const address> local_address() const override {
        return std::make_shared<udp_address>(
            helper_->socket().local_endpoint());
    }

private:
    /*!
     * \brief process on read message
     *
     * \param error error
     * \param endpoint endpoint
     * \param data data
     * \param handler handler
     */
    void on_read(const error_info& error,
        const asio::ip::udp::endpoint& endpoint, const message_data& data,
        const on_accept_handler_type& handler) {
        if (error) {
            MPRPC_ERROR(
                logger_, "failed to accept a connection: {}", error.message());
            handler(error_info(error_code::failed_to_accept, error.message()),
                nullptr);
            return;
        }

        MPRPC_TRACE(logger_, "accepted a connection from {}", endpoint);
        handler(error_info(),
            std::make_shared<udp_session>(logger_, helper_, endpoint, data));
    }

    //! logger
    std::shared_ptr<mprpc::logging::logger> logger_;

    //! socket helper
    std::shared_ptr<udp_common> helper_;

    //! configuration
    const udp_acceptor_config config_;
};

}  // namespace impl
}  // namespace udp
}  // namespace transport
}  // namespace mprpc
