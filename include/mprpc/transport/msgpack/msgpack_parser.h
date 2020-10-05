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
 * \brief declaration and implementation of msgpack_parser class
 */
#pragma once

#include <vector>

#include <msgpack.hpp>

#include "mprpc/logging/logger.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/transport/parser.h"

namespace mprpc {
namespace transport {
namespace msgpack {

/*!
 * \brief class of parsers of messages with msgpack only
 */
class msgpack_parser : public parser {
public:
    /*!
     * \brief construct
     *
     * \param logger logger
     */
    explicit msgpack_parser(std::shared_ptr<logging::logger> logger)
        : logger_(std::move(logger)) {}

    //! \copydoc mprpc::transport::parser::parse
    message_data parse(const char* data, std::size_t size) override {
        try {
            visitor v;
            ::msgpack::parse(data, size, v);
            return message_data(data, size);
        } catch (const exception& e) {
            MPRPC_ERROR(logger_, e.what());
            throw;
        }
    }

private:
    //! visitor
    class visitor : public ::msgpack::null_visitor {
    public:
        //! visit parse error
        [[noreturn]] void parse_error(  // NOLINT: use of external library
            size_t /*parsed_offset*/, size_t /*error_offset*/) {
            throw exception(error_info(
                error_code::parse_error, "failed to parse a message"));
        }

        //! visit error of insufficient bytes
        void insufficient_bytes(  // NOLINT: use of external library
            size_t /*parsed_offset*/, size_t /*error_offset*/) {
            parse_error(0, 0);
        }
    };

    //! logger
    std::shared_ptr<logging::logger> logger_;
};

/*!
 * \brief class of parsers of messages with msgpack and streaming
 */
class msgpack_streaming_parser : public streaming_parser {
public:
    /*!
     * \brief construct
     *
     * \param logger logger
     */
    explicit msgpack_streaming_parser(std::shared_ptr<logging::logger> logger)
        : logger_(std::move(logger)) {}

    //! \copydoc mprpc::transport::streaming_parser::prepare_buffer
    void prepare_buffer(std::size_t size) override {
        buffer_.resize(consumed_buf_size_ + size);
    }

    //! \copydoc mprpc::transport::streaming_parser::buffer
    char* buffer() override { return &buffer_[consumed_buf_size_]; }

    //! \copydoc mprpc::transport::streaming_parser::parse_next
    bool parse_next(std::size_t consumed_buf_size) override {
        try {
            consumed_buf_size_ += consumed_buf_size;
            std::size_t offset = 0;
            bool parse_result = ::msgpack::parse(
                buffer_.data(), consumed_buf_size_, offset, visitor_);
            if (parse_result) {
                parsed_buf_size_ = offset;
            }
            return parse_result;
        } catch (const exception& e) {
            MPRPC_ERROR(logger_, e.what());
            throw;
        }
    }

    //! \copydoc mprpc::transport::streaming_parser::get
    message_data get() override {
        auto data = message_data(buffer_.data(), parsed_buf_size_);
        buffer_.erase(buffer_.begin(), buffer_.begin() + parsed_buf_size_);
        consumed_buf_size_ -= parsed_buf_size_;
        parsed_buf_size_ = 0;
        return data;
    }

private:
    //! visitor
    class visitor : public ::msgpack::null_visitor {
    public:
        //! visit parse error
        [[noreturn]] void parse_error(  // NOLINT: use of external library
            size_t /*parsed_offset*/, size_t /*error_offset*/) {
            throw exception(error_info(
                error_code::parse_error, "failed to parse a message"));
        }
    };

    //! logger
    std::shared_ptr<logging::logger> logger_;

    //! buffer
    std::vector<char> buffer_{};

    //! size of consumed buffer
    std::size_t consumed_buf_size_{0};

    //! size of parsed buffer
    std::size_t parsed_buf_size_{0};

    //! visitor
    visitor visitor_{};
};

}  // namespace msgpack
}  // namespace transport
}  // namespace mprpc
