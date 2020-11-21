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
 * \brief declaration and implementation of configuration of zstd-based
 * compressors
 */
#pragma once

#include "mprpc/config/option.h"
#include "mprpc/mprpc_export.h"

namespace mprpc {
namespace transport {
namespace compressors {
namespace impl {

/*!
 * \brief validate compression levels in zstd library
 *
 * \param level compression level
 * \return whether the compression level is valid
 */
MPRPC_EXPORT bool validate_zstd_compression_level(int level);

}  // namespace impl

/*!
 * \brief configuration option type for compression levels in zstd library
 */
struct zstd_compression_level_type {
    //! value type
    using value_type = int;

    /*!
     * \brief get default value
     *
     * \return default value
     */
    static value_type default_value() noexcept {
        constexpr int value = 3;
        return value;
    }

    /*!
     * \brief validate a value
     *
     * \param value value
     * \return whether the value is valid
     */
    static bool validate(value_type value) noexcept {
        return impl::validate_zstd_compression_level(value);
    }

    /*!
     * \brief get option name
     *
     * \return option name
     */
    static std::string name() { return "zstd_compression_level"; }

    /*!
     * \brief get description
     *
     * \return description
     */
    static std::string description() {
        return "compression levels in zstd library";
    }
};

}  // namespace compressors
}  // namespace transport
}  // namespace mprpc
