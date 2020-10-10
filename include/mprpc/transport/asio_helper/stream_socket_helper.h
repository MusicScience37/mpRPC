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
 * \brief declaration and implementation of stream_socket_helper class
 */
#pragma once

#include <queue>

#include <asio/bind_executor.hpp>
#include <asio/post.hpp>
#include <asio/strand.hpp>
#include <asio/write.hpp>

#include "mprpc/exception.h"
#include "mprpc/logging/logger.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/thread_pool.h"
#include "mprpc/transport/parser.h"

namespace mprpc {
namespace transport {
namespace asio_helper {

/*!
 * \brief helper class to process streaming sockets
 *
 * \tparam SocketType type of socket
 */
template <typename SocketType>
class stream_socket_helper {
public:
    //! type of socket
    using socket_type = SocketType;

    //! type of handlers on reading a message
    using on_read_handler_type =
        std::function<void(const error_info&, const message_data&)>;

    //! type of handlers on writing a message
    using on_write_handler_type = std::function<void(const error_info&)>;

    /*!
     * \brief construct
     *
     * \param logger logger
     * \param socket socket
     * \param io_context io_context
     * \param parser parser
     */
    stream_socket_helper(std::shared_ptr<logging::logger> logger,
        socket_type socket, asio::io_context& io_context,
        std::shared_ptr<streaming_parser> parser)
        : logger_(std::move(logger)),
          socket_(std::move(socket)),
          strand_(asio::make_strand(io_context)),
          parser_(std::move(parser)) {}

    /*!
     * \brief asynchronously read a message
     *
     * \param handler handler
     */
    void async_read(on_read_handler_type handler) {
        asio::post(
            strand_, [this, moved_handler = std::move(handler)]() mutable {
                this->do_read_message(std::move(moved_handler));
            });
    }

    /*!
     * \brief asynchronously write a message
     *
     * \param data message data
     * \param handler handler
     */
    void async_write(const message_data& data, on_write_handler_type handler) {
        asio::post(
            strand_, [this, data, move_handler = std::move(handler)]() mutable {
                this->add_message_to_write(data, std::move(move_handler));
            });
    }

    /*!
     * \brief access to socket
     *
     * \return socket
     */
    const socket_type& socket() const { return socket_; }

private:
    /*!
     * \brief read a message
     *
     * \param handler handler
     */
    void do_read_message(
        on_read_handler_type handler) {  // NOLINT: false positive
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
        static constexpr std::size_t min_buf_size = 1024;
        const auto buf_size = std::max(min_buf_size, socket_.available());
        parser_->prepare_buffer(buf_size);
        socket_.async_read_some(asio::buffer(parser_->buffer(), buf_size),
            asio::bind_executor(strand_,
                [this, moved_handler = std::move(handler)](
                    const asio::error_code& err,
                    std::size_t num_bytes) mutable {
                    this->on_read_bytes(
                        err, num_bytes, std::move(moved_handler));
                }));
    }

    /*!
     * \brief process on read some bytes
     *
     * \param err error code
     * \param num_bytes number of bytes read
     * \param handler handler
     */
    void on_read_bytes(const asio::error_code& err, std::size_t num_bytes,
        on_read_handler_type handler) {  // NOLINT: false positive
        if (err) {
            if (err == asio::error::operation_aborted) {
                return;
            }
            if (err == asio::error::eof) {
                MPRPC_INFO(logger_, "connection closed");
                handler(error_info(error_code::eof, "connection closed"),
                    message_data());
                return;
            }
            MPRPC_ERROR(
                logger_, "error reading from socket: {}", err.message());
            handler(error_info(error_code::failed_to_read, err.message()),
                message_data());
            return;
        }

        MPRPC_TRACE(logger_, "received {} bytes", num_bytes);

        if (socket_.available() == 0) {
            MPRPC_TRACE(
                logger_, "no data available currently, so prase data once");
            if (parse_next(handler, num_bytes)) {
                return;
            }
        } else {
            parser_->consumed(num_bytes);
        }
        MPRPC_TRACE(logger_, "read more bytes");

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
                auto message = parser_->get();
                MPRPC_DEBUG(logger_, "received a message with {} bytes",
                    message.size());
                MPRPC_TRACE(logger_, "received message: {}", message);
                handler(error_info(), message);
                return true;
            }
        } catch (const exception& e) {
            MPRPC_ERROR(logger_, "{}", e.info());
            handler(e.info(), message_data());
            return true;
        }
        return false;
    }

    /*!
     * \brief add a message to write
     *
     * \param data data
     * \param handler handler
     */
    void add_message_to_write(
        const message_data& data, on_write_handler_type handler) {
        MPRPC_TRACE(logger_, "add a message to write");
        bool start_writing = writing_queue_.empty();
        writing_queue_.emplace(data, std::move(handler));
        if (start_writing) {
            do_write();
        }
    }

    /*!
     * \brief write a message
     */
    void do_write() {
        const auto& data = writing_queue_.front().first;
        MPRPC_TRACE(logger_, "starting to write a message");
        asio::async_write(socket_, asio::buffer(data.data(), data.size()),
            asio::bind_executor(strand_,
                [this](const asio::error_code& error,
                    std::size_t /*num_bytes*/) { this->on_write(error); }));
    }

    /*!
     * \brief process on a message written
     *
     * \param error error
     */
    void on_write(const asio::error_code& error) {
        if (error == asio::error::operation_aborted) {
            return;
        }

        const auto& data = writing_queue_.front().first;
        const auto& handler = writing_queue_.front().second;
        if (error) {
            MPRPC_ERROR(logger_, error.message());
            handler(error_info(error_code::failed_to_write, error.message()));
            return;
        }

        MPRPC_DEBUG(logger_, "wrote a message with {} bytes", data.size());
        MPRPC_TRACE(logger_, "written message: {}", data);
        handler(error_info());

        writing_queue_.pop();
        if (writing_queue_.empty()) {
            return;
        }
        do_write();
    }

    //! logger
    std::shared_ptr<logging::logger> logger_;

    //! socket
    socket_type socket_;

    //! strand
    asio::strand<asio::io_context::executor_type> strand_;

    //! queue of messages to be written
    std::queue<std::pair<message_data, on_write_handler_type>> writing_queue_{};

    //! parser of messages
    std::shared_ptr<streaming_parser> parser_;
};

}  // namespace asio_helper
}  // namespace transport
}  // namespace mprpc