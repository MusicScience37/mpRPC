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
 * \brief implementation of zstd-based parser classes
 */
#pragma once

#include <zstd.h>

#include "mprpc/buffer.h"
#include "mprpc/logging/logger.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/transport/parser.h"
#include "mprpc/transport/parsers/impl/msgpack_parser_impl.h"

namespace mprpc {
namespace transport {
namespace parsers {
namespace impl {

/*!
 * \brief class of parsers of messages using zstd library without streaming
 */
class zstd_parser final : public parser {
public:
    /*!
     * \brief construct
     *
     * \param logger logger
     */
    explicit zstd_parser(std::shared_ptr<logging::logger> logger)
        : logger_(std::move(logger)) {
        context_ = ZSTD_createDCtx();
    }

    //! \copydoc mprpc::transport::parser::parse
    message_data parse(const char* data, std::size_t size) override {
        buffer_.resize(ZSTD_getDecompressedSize(data, size));

        MPRPC_TRACE(logger_, "decompressing a message using zstd library");

        auto result = ZSTD_decompressDCtx(
            context_, buffer_.data(), buffer_.size(), data, size);
        if (ZSTD_isError(result) != 0U) {
            MPRPC_ERROR(logger_, "error in decompression with zstd library: {}",
                ZSTD_getErrorName(result));
            throw exception(error_info(error_code::invalid_message,
                std::string() + "error in decompression with zstd library: " +
                    ZSTD_getErrorName(result)));
        }

        MPRPC_TRACE(logger_,
            "decompressed a message from {} bytes to {} bytes using zstd "
            "library",
            size, result);

        return message_data(buffer_.data(), result);
    }

    //! destruct
    ~zstd_parser() override { ZSTD_freeDCtx(context_); }

    zstd_parser(const zstd_parser&) = delete;
    zstd_parser(zstd_parser&&) = delete;
    zstd_parser& operator=(const zstd_parser&) = delete;
    zstd_parser& operator=(zstd_parser&&) = delete;

private:
    //! logger
    std::shared_ptr<logging::logger> logger_;

    //! buffer
    mprpc::buffer buffer_{};

    //! context
    ZSTD_DCtx* context_{nullptr};
};

/*!
 * \brief class of parsers of messages using zstd library with streaming
 */
class zstd_streaming_parser final : public streaming_parser {
public:
    /*!
     * \brief construct
     *
     * \param logger logger
     */
    explicit zstd_streaming_parser(
        const std::shared_ptr<logging::logger>& logger)
        : logger_(logger), msgpack_parser_(logger) {
        context_ = ZSTD_createDCtx();
        input_ = ZSTD_inBuffer{buffer_.data(), 0, 0};
    }

    //! \copydoc mprpc::transport::streaming_parser::prepare_buffer
    void prepare_buffer(std::size_t size) override {
        buffer_.resize(input_.size + size);
        input_.src = buffer_.data();
    }

    //! \copydoc mprpc::transport::streaming_parser::buffer
    char* buffer() override { return buffer_.data() + input_.size; }

    //! \copydoc mprpc::transport::streaming_parser::consumed
    void consumed(std::size_t consumed_buf_size) override {
        input_.size += consumed_buf_size;
    }

    //! \copydoc mprpc::transport::streaming_parser::parse_next
    bool parse_next(std::size_t consumed_buf_size) override {
        consumed(consumed_buf_size);

        MPRPC_TRACE(logger_, "decompressing a message using zstd library");

        std::size_t decompressed_bytes = 0;
        while (input_.pos < input_.size) {
            static const auto zstd_default_output_size = ZSTD_DStreamOutSize();
            MPRPC_TRACE(logger_, "preparing buffer with {} bytes",
                zstd_default_output_size);
            msgpack_parser_.prepare_buffer(zstd_default_output_size);

            auto output = ZSTD_outBuffer{
                msgpack_parser_.buffer(), zstd_default_output_size, 0};
            auto result = ZSTD_decompressStream(context_, &output, &input_);
            if (ZSTD_isError(result) != 0U) {
                MPRPC_ERROR(logger_,
                    "error in decompression with zstd library: {}",
                    ZSTD_getErrorName(result));
                throw exception(error_info(error_code::invalid_message,
                    std::string() +
                        "error in decompression with zstd library: " +
                        ZSTD_getErrorName(result)));
            }

            MPRPC_TRACE(
                logger_, "output of ZSTD_decompressStream is {}", result);

            if (output.pos > 0) {
                decompressed_bytes += output.pos;
                msgpack_parser_.consumed(output.pos);
            }

            if ((input_.pos == input_.size) || (output.pos < output.size)) {
                // completed current input
                break;
            }
        }

        MPRPC_TRACE(logger_, "decompressed {} bytes in inputs to {} bytes",
            input_.pos, decompressed_bytes);

        buffer_.consume(input_.pos);
        input_.size -= input_.pos;
        input_.pos = 0;

        return msgpack_parser_.parse_next(0);
    }

    //! \copydoc mprpc::transport::streaming_parser::get
    message_data get() override { return msgpack_parser_.get(); }

    //! destruct
    ~zstd_streaming_parser() override { ZSTD_freeDCtx(context_); }

    zstd_streaming_parser(const zstd_streaming_parser&) = delete;
    zstd_streaming_parser(zstd_streaming_parser&&) = delete;
    zstd_streaming_parser& operator=(const zstd_streaming_parser&) = delete;
    zstd_streaming_parser& operator=(zstd_streaming_parser&&) = delete;

private:
    //! logger
    std::shared_ptr<logging::logger> logger_;

    //! buffer of input data
    mprpc::buffer buffer_{};

    //! input for zstd library
    ZSTD_inBuffer input_{};

    //! parser of MessagePack data
    msgpack_streaming_parser msgpack_parser_;

    //! context
    ZSTD_DCtx* context_{nullptr};
};

}  // namespace impl
}  // namespace parsers
}  // namespace transport
}  // namespace mprpc
