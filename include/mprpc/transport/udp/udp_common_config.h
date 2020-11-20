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
 * \brief declaration and implementation of udp_common_config class
 */
#pragma once

#include "mprpc/config/option.h"

namespace mprpc {
namespace transport {
namespace udp {

/*!
 * \brief configuration option type for buffer size in datagrams
 */
struct datagram_buf_size_type {
    //! value type
    using value_type = std::size_t;

    /*!
     * \brief get default value
     *
     * \return default value
     */
    static value_type default_value() noexcept {
        constexpr std::size_t value = 65527;  // limit of UDP
        return value;
    }

    /*!
     * \brief validate a value
     *
     * \param value value
     * \return whether the value is valid
     */
    static bool validate(value_type value) noexcept { return value > 0; }

    /*!
     * \brief get option name
     *
     * \return option name
     */
    static std::string name() { return "datagram_buf_size"; }

    /*!
     * \brief get description
     *
     * \return description
     */
    static std::string description() { return "buffer size in datagrams"; }
};

/*!
 * \brief common configuration of UDP
 */
struct udp_common_config {
    //! buffer size in datagrams
    config::option<datagram_buf_size_type> datagram_buf_size{};
};

}  // namespace udp
}  // namespace transport
}  // namespace mprpc
