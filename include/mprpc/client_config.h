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
 * \brief declaration and implementation of client_config class
 */
#pragma once

#include <cstdint>

#include "mprpc/config/option.h"
#include "mprpc/num_threads.h"
#include "mprpc/transport.h"

namespace mprpc {

/*!
 * \brief configuration option type for timeout of synchronous requests [ms]
 */
struct sync_request_timeout_ms_type {
    //! value type
    using value_type = std::uint32_t;

    /*!
     * \brief get default value
     *
     * \return default value
     */
    static value_type default_value() noexcept {
        constexpr std::uint32_t value = 3000;
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
    static std::string name() { return "sync_request_timeout_ms"; }

    /*!
     * \brief get description
     *
     * \return description
     */
    static std::string description() {
        return "timeout of synchronous requests [ms]";
    }
};

/*!
 * \brief enumeration of transport types
 */
enum class transport_type {
    tcp,  //!< TCP
    udp   //!< UDP
};

/*!
 * \brief configuration option type for connector types
 */
struct connector_type_type {
    //! value type
    using value_type = transport_type;

    /*!
     * \brief get default value
     *
     * \return default value
     */
    static value_type default_value() { return transport_type::tcp; }

    /*!
     * \brief validate a value
     *
     * \param value value
     * \return whether the value is valid
     */
    static bool validate(const value_type& value) {
        return (value == transport_type::tcp) || (value == transport_type::udp);
    }

    /*!
     * \brief get option name
     *
     * \return option name
     */
    static std::string name() { return "connector_type"; }

    /*!
     * \brief get description
     *
     * \return description
     */
    static std::string description() { return "connector type (TCP, UDP)"; }
};

/*!
 * \brief configuration of clients
 */
struct client_config {
    //! number of threads
    config::option<num_threads_type> num_threads{};

    //! timeout of synchronous requests [ms]
    config::option<sync_request_timeout_ms_type> sync_request_timeout_ms{};

    //! transport type
    config::option<connector_type_type> connector_type{};

    //! TCP connector configuration
    transport::tcp::tcp_connector_config tcp_connector{};

    //! UDP connector configuration
    transport::udp::udp_connector_config udp_connector{};
};

}  // namespace mprpc
