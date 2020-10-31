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

#include <asio/bind_executor.hpp>
#include <asio/ip/udp.hpp>
#include <asio/post.hpp>
#include <asio/strand.hpp>
#include <stl_ext/shared_function.h>

#include "mprpc/logging/logger.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/thread_pool.h"
#include "mprpc/transport/asio_helper/basic_endpoint.h"
#include "mprpc/transport/parser.h"
#include "mprpc/transport/udp/impl/udp_address.h"
#include "mprpc/transport/udp/udp_common_config.h"

namespace mprpc {
namespace transport {
namespace udp {

/*!
 * \brief class to perform common processes on UDP sockets
 */
class udp_common : public std::enable_shared_from_this<udp_common> {
public:
    //! type of handlers on reading a message
    using on_read_handler_type =
        stl_ext::shared_function<void(const error_info&,
            const asio::ip::udp::endpoint&, const message_data&)>;

    //! type of handlers on writing a message
    using on_write_handler_type =
        stl_ext::shared_function<void(const error_info&)>;

    /*!
     * \brief construct
     *
     * \param logger logger
     * \param socket socket
     * \param io_context io_context
     * \param parser_ptr parser
     * \param config configuration
     */
    udp_common(std::shared_ptr<logging::logger> logger,
        asio::ip::udp::socket socket, asio::io_context& io_context,
        std::shared_ptr<parser> parser_ptr, const udp_common_config& config)
        : logger_(std::move(logger)),
          socket_(std::move(socket)),
          strand_(asio::make_strand(io_context)),
          parser_(std::move(parser_ptr)),
          config_(config) {}

    /*!
     * \brief initialize
     */
    void init() { buf_.resize(config_.datagram_buf_size.value()); }

    /*!
     * \brief asynchronously read a message
     *
     * \param handler handler
     */
    void async_read(const on_read_handler_type& handler) {
        asio::post(strand_, [weak_self = weak_from_this(), handler] {
            const auto self = weak_self.lock();
            if (self) {
                self->do_read(handler);
            }
        });
    }

    /*!
     * \brief asynchronously write a message
     *
     * \param endpoint endpoint
     * \param msg message
     * \param handler handler
     */
    void async_write(const asio::ip::udp::endpoint& endpoint,
        const message_data& msg, const on_write_handler_type& handler) {
        asio::post(
            strand_, [weak_self = weak_from_this(), endpoint, msg, handler] {
                const auto self = weak_self.lock();
                if (self) {
                    self->do_write(endpoint, msg, handler);
                }
            });
    }

    /*!
     * \brief get socket
     *
     * \return socket
     */
    const asio::ip::udp::socket& socket() const noexcept { return socket_; }

private:
    /*!
     * \brief asynchronously read a message
     *
     * \param handler handler
     */
    void do_read(const on_read_handler_type& handler) {
        MPRPC_TRACE(logger_, "receive next packet");
        socket_.async_receive_from(asio::buffer(buf_.data(), buf_.size()),
            sender_,
            asio::bind_executor(strand_,
                [weak_self = weak_from_this(), handler](
                    const asio::error_code& error, std::size_t num_bytes) {
                    const auto self = weak_self.lock();
                    if (self) {
                        self->on_read(error, num_bytes, handler);
                    }
                }));
    }

    /*!
     * \brief process on read message
     *
     * \param error error
     * \param num_bytes number of bytes
     * \param handler handler
     */
    void on_read(const asio::error_code& error, std::size_t num_bytes,
        const on_read_handler_type& handler) {
        if (error) {
            if (error == asio::error::operation_aborted) {
                return;
            }
            if (error == asio::error::eof) {
                MPRPC_INFO(logger_, "connection closed");
                handler(error_info(error_code::eof, "connection closed"),
                    asio::ip::udp::endpoint(), message_data());
                return;
            }
            MPRPC_ERROR(
                logger_, "error reading from socket: {}", error.message());
            handler(error_info(error_code::failed_to_read, error.message()),
                asio::ip::udp::endpoint(), message_data());
            return;
        }

        MPRPC_TRACE(logger_, "received {} bytes", num_bytes);

        try {
            const auto msg = parser_->parse(buf_.data(), num_bytes);
            MPRPC_DEBUG(
                logger_, "received a message with {} bytes", msg.size());
            MPRPC_TRACE(logger_, "received message: {}", msg);
            handler(error_info(), sender_, msg);
        } catch (const exception& e) {
            MPRPC_ERROR(logger_, "{}", e.info());
            handler(e.info(), asio::ip::udp::endpoint(), message_data());
        }
    }

    /*!
     * \brief asynchronously write a message
     *
     * \param endpoint endpoint
     * \param msg message
     * \param handler handler
     */
    void do_write(const asio::ip::udp::endpoint& endpoint,
        const message_data& msg, const on_write_handler_type& handler) {
        MPRPC_TRACE(logger_, "send a message");
        socket_.async_send_to(asio::buffer(msg.data(), msg.size()), endpoint,
            asio::bind_executor(strand_,
                [weak_self = weak_from_this(), msg, handler](
                    const asio::error_code& error, std::size_t num_bytes) {
                    const auto self = weak_self.lock();
                    if (self) {
                        self->on_write(error, msg, handler);
                    }
                }));
    }

    /*!
     * \brief process on written message
     *
     * \param error error
     * \param msg message
     * \param handler handler
     */
    void on_write(const asio::error_code& error, const message_data& msg,
        const on_write_handler_type& handler) {
        if (error == asio::error::operation_aborted) {
            return;
        }
        if (error) {
            MPRPC_ERROR(logger_, error.message());
            handler(error_info(error_code::failed_to_write, error.message()));
            return;
        }

        MPRPC_DEBUG(logger_, "sended a message with {} bytes", msg.size());
        MPRPC_TRACE(logger_, "sended message: {}", msg);
        handler(error_info());
    }

    /*!
     * \brief get weak pointer to this
     *
     * \return pointer
     */
    std::weak_ptr<udp_common> weak_from_this() {
        return std::enable_shared_from_this<udp_common>::shared_from_this();
    }

    //! logger
    std::shared_ptr<logging::logger> logger_;

    //! socket
    asio::ip::udp::socket socket_;

    //! strand
    asio::strand<asio::io_context::executor_type> strand_;

    //! parser of messages
    std::shared_ptr<parser> parser_;

    //! received data buffer
    std::vector<char> buf_{};

    //! sender of received data
    asio::ip::udp::endpoint sender_{};

    //! configuration
    const udp_common_config& config_;
};

}  // namespace udp
}  // namespace transport
}  // namespace mprpc
