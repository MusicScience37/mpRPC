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
#include "mprpc/transport/compressors/zstd_compressor.h"

#include "mprpc/transport/compressors/impl/zstd_compressor_impl.h"

namespace mprpc {
namespace transport {
namespace compressors {

std::unique_ptr<compressor> create_zstd_compressor(
    std::shared_ptr<logging::logger> logger, zstd_compressor_config config) {
    return std::make_unique<impl::zstd_compressor>(std::move(logger), config);
}

std::unique_ptr<streaming_compressor> create_zstd_streaming_compressor(
    // NOLINTNEXTLINE: temporary
    std::shared_ptr<logging::logger> logger, zstd_compressor_config config) {
    return nullptr;
}

}  // namespace compressors
}  // namespace transport
}  // namespace mprpc
