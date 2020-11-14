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
 * \brief declaration and implementation of ip_common_config class
 * class
 */
#pragma once

#include <cstdint>
#include <string>

#include "mprpc/config/option.h"

namespace mprpc {
namespace transport {
namespace common {

/*!
 * \brief configuration option type for host addresses
 */
struct host_type {
    //! value type
    using value_type = std::string;

    /*!
     * \brief get default value
     *
     * \return default value
     */
    static value_type default_value() { return "127.0.0.1"; }

    /*!
     * \brief validate a value
     *
     * \return whether the value is valid
     */
    static bool validate(const value_type& /*value*/) noexcept { return true; }

    /*!
     * \brief get option name
     *
     * \return option name
     */
    static std::string name() { return "host"; }
};

/*!
 * \brief configuration option type for port numbers
 */
struct port_type {
    //! value type
    using value_type = std::uint16_t;

    /*!
     * \brief get default value
     *
     * \return default value
     */
    static value_type default_value() {
        constexpr std::uint16_t value = 3780;
        return value;
    }

    /*!
     * \brief validate a value
     *
     * \return whether the value is valid
     */
    static bool validate(value_type /*value*/) noexcept { return true; }

    /*!
     * \brief get option name
     *
     * \return option name
     */
    static std::string name() { return "port"; }
};

/*!
 * \brief common configuration for transport on IP
 */
struct ip_common_config {
    //! host address
    config::option<host_type> host{};

    //! port number
    config::option<port_type> port{};
};

}  // namespace common
}  // namespace transport
}  // namespace mprpc
