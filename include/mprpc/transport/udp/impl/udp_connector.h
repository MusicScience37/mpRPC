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
 * \brief declaration and implementation of udp_connector class
 */
#pragma once

#include <memory>

#include <asio/ip/udp.hpp>

#include "mprpc/logging/logger.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/thread_pool.h"
#include "mprpc/transport/asio_helper/basic_endpoint.h"
#include "mprpc/transport/connector.h"
#include "mprpc/transport/parser.h"
#include "mprpc/transport/udp/impl/udp_address.h"
#include "mprpc/transport/udp/impl/udp_common.h"
#include "mprpc/transport/udp/udp_connector_config.h"

namespace mprpc {
namespace transport {
namespace udp {

/*!
 * \brief class of connectors on UDP sockets
 */
class udp_connector : public connector {
public:
    /*!
     * \brief construct
     *
     * \param logger logger
     * \param socket socket
     * \param endpoint endpoint
     * \param io_context io_context
     * \param comp compressor
     * \param parser_ptr parser
     * \param config configuration
     */
    udp_connector(std::shared_ptr<logging::logger> logger,
        asio::ip::udp::socket socket, asio::ip::udp::endpoint endpoint,
        asio::io_context& io_context, std::shared_ptr<compressor> comp,
        std::shared_ptr<parser> parser_ptr, udp_connector_config config)
        : helper_(
              std::make_shared<udp_common>(std::move(logger), std::move(socket),
                  io_context, std::move(comp), std::move(parser_ptr), config_)),
          endpoint_(std::move(endpoint)),
          config_(config) {
        helper_->init();
    }

    //! \copydoc mprpc::transport::connector::async_read
    void async_read(on_read_handler_type handler) override {
        helper_->async_read(
            [handler](const error_info& error,
                const asio::ip::udp::endpoint& /*endpoint*/,
                const message_data& data) { handler(error, data); });
    }

    //! \copydoc mprpc::transport::connector::async_write
    void async_write(
        const message_data& data, on_write_handler_type handler) override {
        helper_->async_write(endpoint_, data, handler);
    }

    //! \copydoc mprpc::transport::connector::local_address
    std::shared_ptr<const address> local_address() const override {
        return std::make_shared<udp_address>(
            helper_->socket().local_endpoint());
    }

    //! \copydoc mprpc::transport::connector::remote_address
    std::shared_ptr<const address> remote_address() const override {
        return std::make_shared<udp_address>(endpoint_);
    }

private:
    //! socket helper
    std::shared_ptr<udp_common> helper_;

    //! endpoint
    asio::ip::udp::endpoint endpoint_;

    //! configuration
    const udp_common_config config_;
};

}  // namespace udp
}  // namespace transport
}  // namespace mprpc
