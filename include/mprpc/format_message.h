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
 * \brief declaration and implementation of format_message function
 */
#pragma once

#include <ostream>
#include <string>

#include "mprpc/logging/log_data_traits.h"
#include "mprpc/message_data.h"
#include "mprpc/mprpc_export.h"

namespace mprpc {
namespace impl {

/*!
 * \brief default limit of output length of formatted data
 */
static constexpr std::size_t default_formatted_data_length_limit = 100;

}  // namespace impl

/*!
 * \brief format messsage data to readable string
 *
 * \param data message data
 * \param length_limit limit of output length
 * \return formatted data
 */
MPRPC_EXPORT std::string format_message(const message_data& data,
    std::size_t length_limit =
        impl::default_formatted_data_length_limit) noexcept;

namespace logging {

/*!
 * \brief log_data_traits for message_data class
 */
template <>
struct log_data_traits<message_data> {
    /*!
     * \brief preprocess data
     *
     * \param data data
     * \return formatted data
     */
    static std::string preprocess(const message_data& data) noexcept {
        return format_message(data);
    }
};

}  // namespace logging

/*!
 * \brief output data to a stream
 *
 * \param stream stream
 * \param data data
 * \return stream
 */
inline std::ostream& operator<<(
    std::ostream& stream, const message_data& data) {
    stream << format_message(data);
    return stream;
}

}  // namespace mprpc
