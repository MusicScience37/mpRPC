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

namespace mprpc {
namespace transport {
namespace parsers {
namespace impl {

/*!
 * \brief class of parsers of messages using zstd library
 */
class zstd_parser : public parser {
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
            throw exception(error_info(error_code::unexpected_error,
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

}  // namespace impl
}  // namespace parsers
}  // namespace transport
}  // namespace mprpc
