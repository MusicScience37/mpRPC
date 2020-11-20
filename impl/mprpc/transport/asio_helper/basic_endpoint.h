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
 * \brief declaration and implementation of helper for asio::ip::basic_endpoint
 * class
 */
#pragma once

#include <sstream>

#include <asio/ip/basic_endpoint.hpp>

#include "mprpc/logging/log_data_traits.h"

namespace mprpc {
namespace logging {

/*!
 * \brief log_data_traits for asio::ip::basic_endpoint
 *
 * \tparam InternetProtocol protocol
 */
template <typename InternetProtocol>
struct log_data_traits<asio::ip::basic_endpoint<InternetProtocol>> {
    //! data type
    using data_type = asio::ip::basic_endpoint<InternetProtocol>;

    /*!
     * \brief preprocess data
     *
     * \param data data
     * \return formatted data
     */
    static std::string preprocess(const data_type& data) noexcept {
        std::ostringstream stream;
        stream << data;
        return stream.str();
    }
};

}  // namespace logging
}  // namespace mprpc
