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
 * \brief declaration and implementation of option class
 */
#pragma once

#include <stdexcept>

#include <toml.hpp>

#include "mprpc/config/option.h"
#include "mprpc/transport/compression_config.h"

namespace toml {

/*!
 * \brief toml::from specialization for mprpc::config::option
 *
 * \tparam OptionType option type
 */
template <typename OptionType>
struct from<mprpc::config::option<OptionType>> {
    /*!
     * \brief convert from toml value
     *
     * \tparam Comment comment type
     * \tparam Table table type
     * \tparam Array array type
     * \param value toml value
     * \return converted value
     */
    template <typename Comment, template <typename...> class Table,
        template <typename...> class Array>
    static mprpc::config::option<OptionType> from_toml(
        const basic_value<Comment, Table, Array>& value) {
        using value_type = typename OptionType::value_type;
        const value_type converted = toml::get<value_type>(value);
        if (!OptionType::validate(converted)) {
            throw std::runtime_error(format_error("invalid value for " +
                    OptionType::name() + ", " + OptionType::description(),
                value, "invalid value"));
        }
        mprpc::config::option<OptionType> option;
        option = converted;
        return option;
    }
};

}  // namespace toml
