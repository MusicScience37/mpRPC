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

#include <asio/ip/tcp.hpp>

#include "mprpc/exception.h"
#include "mprpc/logging/logger.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/thread_pool.h"
#include "mprpc/transport/parser.h"
#include "mprpc/transport/session.h"

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
     * \param threads thread pool
     * \param parser parser
     */
    tcp_session(std::shared_ptr<logging::logger> logger,
        asio::ip::tcp::socket socket, std::shared_ptr<thread_pool> threads,
        std::shared_ptr<streaming_parser> parser)
        : logger_(std::move(logger)),
          socket_(std::move(socket)),
          threads_(std::move(threads)),
          parser_(std::move(parser)) {
        MPRPC_INFO(logger_, "session created");
    }

    //! \copydoc mprpc::transport::session::async_read
    void async_read(on_read_handler_type handler) override {
        threads_->post([self = shared_from_this(),
                           moved_handler = std::move(handler)]() mutable {
            std::static_pointer_cast<tcp_session>(self)->do_read_message(
                std::move(moved_handler));
        });
    }

    /*!
     * \brief read a message
     *
     * \param handler handler
     */
    void do_read_message(on_read_handler_type handler) {
        if (parse_next(handler, 0)) {
            return;
        }

        do_read_bytes(std::move(handler));
    }

    /*!
     * \brief read some bytes
     *
     * \param handler handler
     */
    void do_read_bytes(on_read_handler_type handler) {
        static constexpr std::size_t buf_size = 128;
        parser_->prepare_buffer(buf_size);
        socket_.async_read_some(asio::buffer(parser_->buffer(), buf_size),
            [self = shared_from_this(), moved_handler = std::move(handler)](
                const asio::error_code& err, std::size_t num_bytes) mutable {
                std::static_pointer_cast<tcp_session>(self)->on_read_bytes(
                    err, num_bytes, std::move(moved_handler));
            });
    }

    /*!
     * \brief process on read some bytes
     *
     * \param err error code
     * \param num_bytes number of bytes read
     * \param handler handler
     */
    void on_read_bytes(const asio::error_code& err, std::size_t num_bytes,
        on_read_handler_type handler) {
        if (err) {
            if (err == asio::error::eof) {
                MPRPC_INFO(logger_, "session closed");
                handler(error_info(error_code::eof, "session closed"),
                    message_data("", 0));
                return;
            }
            MPRPC_ERROR(logger_, err.message());
            handler(error_info(error_code::failed_to_read, err.message()),
                message_data("", 0));
            return;
        }

        MPRPC_TRACE(logger_, "read {} bytes", num_bytes);

        if (parse_next(handler, num_bytes)) {
            return;
        }

        do_read_bytes(std::move(handler));
    }

    /*!
     * \brief parse next message
     *
     * \param handler handler
     * \param num_bytes number of bytes
     * \return whether to finish the process
     */
    bool parse_next(
        const on_read_handler_type& handler, std::size_t num_bytes) {
        try {
            if (parser_->parse_next(num_bytes)) {
                MPRPC_DEBUG(logger_, "parsed a message");
                handler(error_info(), parser_->get());
                return true;
            }
        } catch (const exception& e) {
            MPRPC_ERROR(logger_, "{}", e.info());
            handler(e.info(), message_data("", 0));
            return true;
        }
        return false;
    }

    //! \copydoc mprpc::transport::session::async_write
    void async_write(
        const message_data& data, on_write_handler_type handler) override {
        //! \todo implementation
    }

    //! \copydoc mprpc::transport::session::remote_address
    std::shared_ptr<address> remote_address() const override {
        //! \todo implementation
        return nullptr;
    }

private:
    //! logger
    std::shared_ptr<logging::logger> logger_;

    //! socket
    asio::ip::tcp::socket socket_;

    //! thread pool
    std::shared_ptr<thread_pool> threads_;

    //! parser of messages
    std::shared_ptr<streaming_parser> parser_;
};

}  // namespace tcp
}  // namespace transport
}  // namespace mprpc
