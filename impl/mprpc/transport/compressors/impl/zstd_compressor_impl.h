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
 * \brief implementation of zstd-based compressor classes
 */
#pragma once

#include <zstd.h>

#include "mprpc/buffer.h"
#include "mprpc/exception.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/transport/compression_config.h"
#include "mprpc/transport/compressor.h"

namespace mprpc {
namespace transport {
namespace compressors {
namespace impl {

/*!
 * \brief class of compressors of messages using zstd without streaming
 */
class zstd_compressor final : public compressor {
public:
    /*!
     * \brief construct
     *
     * \param logger logger
     * \param config configuration
     */
    zstd_compressor(logging::labeled_logger logger, compression_config config)
        : logger_(std::move(logger)), config_(config) {
        context_ = ZSTD_createCCtx();
        ZSTD_CCtx_setParameter(context_, ZSTD_c_compressionLevel,
            config_.zstd_compression_level.value());
    }

    //! \copydoc mprpc::transport::compressor::compress
    shared_binary compress(message_data data) override {
        buffer_.resize(ZSTD_compressBound(data.size()));

        MPRPC_TRACE(logger_, "compressing a message using zstd library");

        auto result = ZSTD_compress2(
            context_, buffer_.data(), buffer_.size(), data.data(), data.size());
        if (ZSTD_isError(result) != 0U) {
            MPRPC_ERROR(logger_, "error in compression with zstd library: {}",
                ZSTD_getErrorName(result));
            throw exception(error_info(error_code::unexpected_error,
                std::string() + "error in compression with zstd library: " +
                    ZSTD_getErrorName(result)));
        }

        MPRPC_TRACE(logger_,
            "compressed a message from {} bytes to {} bytes using zstd library",
            data.size(), result);

        return shared_binary(buffer_.data(), result);
    }

    //! destruct
    ~zstd_compressor() override { ZSTD_freeCCtx(context_); }

    zstd_compressor(const zstd_compressor&) = delete;
    zstd_compressor(zstd_compressor&&) = delete;
    zstd_compressor& operator=(const zstd_compressor&) = delete;
    zstd_compressor& operator=(zstd_compressor&&) = delete;

private:
    //! logger
    logging::labeled_logger logger_;

    //! buffer
    mprpc::buffer buffer_{};

    //! context
    ZSTD_CCtx* context_{nullptr};

    //! configuration
    const compression_config config_;
};

/*!
 * \brief class of compressors of messages using zstd with streaming
 */
class zstd_streaming_compressor final : public streaming_compressor {
public:
    /*!
     * \brief construct
     *
     * \param logger logger
     * \param config configuration
     */
    zstd_streaming_compressor(
        logging::labeled_logger logger, compression_config config)
        : logger_(std::move(logger)), config_(config) {
        context_ = ZSTD_createCCtx();
        ZSTD_CCtx_setParameter(context_, ZSTD_c_compressionLevel,
            config_.zstd_compression_level.value());
    }

    //! \copydoc mprpc::transport::streaming_compressor::init
    void init() override {
        ZSTD_CCtx_reset(context_, ZSTD_reset_session_only);
        ZSTD_CCtx_setParameter(context_, ZSTD_c_compressionLevel,
            config_.zstd_compression_level.value());
    }

    //! \copydoc mprpc::transport::streaming_compressor::compress
    shared_binary compress(message_data data) override {
        const auto buffer_size = ZSTD_compressBound(data.size());
        MPRPC_TRACE(logger_, "preparing buffer with {} bytes", buffer_size);
        buffer_.resize(buffer_size);

        MPRPC_TRACE(logger_, "compressing a message using zstd library");

        auto input = ZSTD_inBuffer{data.data(), data.size(), 0};
        auto output = ZSTD_outBuffer{buffer_.data(), buffer_.size(), 0};

        auto result =
            ZSTD_compressStream2(context_, &output, &input, ZSTD_e_end);
        if (ZSTD_isError(result) != 0U) {
            MPRPC_ERROR(logger_, "error in compression with zstd library: {}",
                ZSTD_getErrorName(result));
            throw exception(error_info(error_code::unexpected_error,
                std::string() + "error in compression with zstd library: " +
                    ZSTD_getErrorName(result)));
        }
        MPRPC_TRACE(logger_, "output of ZSTD_compressStream2 is {}", result);

        MPRPC_TRACE(logger_,
            "compressed a message from {} bytes to {} bytes using zstd library",
            data.size(), output.pos);

        return shared_binary(buffer_.data(), output.pos);
    }

    //! destruct
    ~zstd_streaming_compressor() override { ZSTD_freeCCtx(context_); }

    zstd_streaming_compressor(const zstd_streaming_compressor&) = delete;
    zstd_streaming_compressor(zstd_streaming_compressor&&) = delete;
    zstd_streaming_compressor& operator=(
        const zstd_streaming_compressor&) = delete;
    zstd_streaming_compressor& operator=(zstd_streaming_compressor&&) = delete;

private:
    //! logger
    logging::labeled_logger logger_;

    //! buffer
    mprpc::buffer buffer_{};

    //! context
    ZSTD_CCtx* context_{nullptr};

    //! configuration
    const compression_config config_;
};

}  // namespace impl
}  // namespace compressors
}  // namespace transport
}  // namespace mprpc
