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
 * \brief declaration of zstd-based compressor classes
 */
#pragma once

#include "mprpc/mprpc_export.h"
#include "mprpc/transport/compressor.h"
#include "mprpc/transport/compressors/zstd_compressor_config.h"

namespace mprpc {
namespace transport {
namespace compressors {

/*!
 * \brief create a compressor using zstd library without streaming
 *
 * \param logger logger
 * \param config configuration
 * \return compressor
 */
MPRPC_EXPORT std::unique_ptr<compressor> create_zstd_compressor(
    std::shared_ptr<logging::logger> logger, zstd_compressor_config config);

/*!
 * \brief create a compressor using zstd library with streaming
 *
 * \param logger logger
 * \param config configuration
 * \return compressor
 */
MPRPC_EXPORT std::unique_ptr<streaming_compressor>
create_zstd_streaming_compressor(
    std::shared_ptr<logging::logger> logger, zstd_compressor_config config);

/*!
 * \brief class of factories of compressors using zstd library
 */
class zstd_compressor_factory : public compressor_factory {
public:
    /*!
     * \brief construct
     *
     * \param config configuration
     */
    explicit zstd_compressor_factory(zstd_compressor_config config)
        : config_(config) {}

    //! \copydoc mprpc::transport::compressor_factory::create_compressor
    std::unique_ptr<compressor> create_compressor(
        std::shared_ptr<logging::logger> logger) override {
        return create_zstd_compressor(logger, config_);
    }

    //! \copydoc transport::compressor_factory::create_streaming_compressor
    std::unique_ptr<streaming_compressor> create_streaming_compressor(
        std::shared_ptr<logging::logger> logger) override {
        return create_zstd_streaming_compressor(logger, config_);
    }

private:
    //! configuration
    zstd_compressor_config config_;
};

}  // namespace compressors
}  // namespace transport
}  // namespace mprpc
