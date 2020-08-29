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
 * \brief declaration and implementation of format_log_data function
 */
#pragma once

#include <type_traits>
#include <utility>

#include <fmt/core.h>

#include "mprpc/logging/log_data_traits.h"

namespace mprpc {
namespace logging {

/*!
 * \brief format data for logs
 *
 * \tparam Format format type
 * \tparam Data data types
 * \param format format
 * \param data data
 * \return formatted data
 */
template <typename Format, typename... Data>
std::string format_log_data(const Format& format, Data&&... data) {
    return fmt::format(format,
        log_data_traits<typename std::decay<Data>::type>::preprocess(
            std::forward<Data>(data))...);
}

/*!
 * \brief format data for log (one argument version)
 *
 * \tparam String string type
 * \param str string
 * \return string (unmodified in this version)
 */
template <typename String>
std::string format_log_data(String&& str) {
    return std::string(std::forward<String>(str));
}

}  // namespace logging
}  // namespace mprpc
