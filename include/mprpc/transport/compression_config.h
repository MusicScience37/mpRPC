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
 * \brief declaration and implementation of compression_config class
 */
#pragma once

#include <string>

#include "mprpc/config/option.h"
#include "mprpc/transport/compressors/zstd_compressor_config.h"

namespace mprpc {
namespace transport {

/*!
 * \brief configuration option type for compression types
 */
struct compression_type_type {
    //! value type
    using value_type = std::string;

    /*!
     * \brief get default value
     *
     * \return default value
     */
    static value_type default_value() { return "none"; }

    /*!
     * \brief validate a value
     *
     * \param value value
     * \return whether the value is valid
     */
    static bool validate(const value_type& value) {
        if (value == "none") {
            return true;
        }
        if (value == "zstd") {
            return true;
        }
        return false;
    }

    /*!
     * \brief get option name
     *
     * \return option name
     */
    static std::string name() { return "compression_type"; }
};

/*!
 * \brief configuration for compression
 */
struct compression_config {
    //! compression type
    config::option<compression_type_type> compression_type{};

    //! compression level
    config::option<compressors::zstd_compression_level_type>
        zstd_compression_level{};
};

}  // namespace transport
}  // namespace mprpc
