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
#include "mprpc/transport/compressor.h"
#include "mprpc/transport/compressors/zstd_compressor_config.h"

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
    zstd_compressor(
        std::shared_ptr<logging::logger> logger, zstd_compressor_config config)
        : logger_(std::move(logger)), config_(config) {
        context_ = ZSTD_createCCtx();
    }

    //! \copydoc mprpc::transport::compressor::compress
    message_data compress(message_data data) override {
        buffer_.resize(ZSTD_compressBound(data.size()));

        MPRPC_TRACE(logger_, "compressing a message using zstd library");

        auto result =
            ZSTD_compressCCtx(context_, buffer_.data(), buffer_.size(),
                data.data(), data.size(), config_.compression_level.value());
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

        return message_data(buffer_.data(), result);
    }

    //! destruct
    ~zstd_compressor() override { ZSTD_freeCCtx(context_); }

    zstd_compressor(const zstd_compressor&) = delete;
    zstd_compressor(zstd_compressor&&) = delete;
    zstd_compressor& operator=(const zstd_compressor&) = delete;
    zstd_compressor& operator=(zstd_compressor&&) = delete;

private:
    //! logger
    std::shared_ptr<logging::logger> logger_;

    //! buffer
    mprpc::buffer buffer_{};

    //! context
    ZSTD_CCtx* context_{nullptr};

    //! configuration
    zstd_compressor_config config_;
};

}  // namespace impl
}  // namespace compressors
}  // namespace transport
}  // namespace mprpc
