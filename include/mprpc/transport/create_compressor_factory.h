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
 * \brief declaration of create_compressor_factory function
 */
#pragma once

#include "mprpc/error_code.h"
#include "mprpc/exception.h"
#include "mprpc/transport/compression_config.h"
#include "mprpc/transport/compressor.h"
#include "mprpc/transport/compressors/null_compressor.h"
#include "mprpc/transport/compressors/zstd_compressor.h"

namespace mprpc {
namespace transport {

/*!
 * \brief create a factory of compressors
 *
 * \param config configuration
 * \return factory of compressors
 */
inline std::shared_ptr<compressor_factory> create_compressor_factory(
    const compression_config& config) {
    switch (config.type.value()) {
    case compression_type::none:
        return std::make_shared<compressors::null_compressor_factory>();
    case compression_type::zstd:
        return std::make_shared<compressors::zstd_compressor_factory>(config);
    default:
        throw exception(error_info(
            error_code::invalid_config_value, "invalid compression type"));
    }
}

}  // namespace transport
}  // namespace mprpc
