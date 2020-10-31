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

#include "mprpc/exception.h"

namespace mprpc {
namespace config {

/*!
 * \brief class of configuration options
 *
 * \tparam OptionType option type
 *
 * OptionType must have following static members:
 *
 * - `value_type`: type of configuration values
 * - `value_type default_value()`: function to get default value
 * - `bool validate(value_type)`: function to validate a value
 * - `std::string name()`: function to get configuration name
 */
template <typename OptionType>
class option {
public:
    //! option type
    using option_type = OptionType;

    //! value type
    using value_type = typename option_type::value_type;

    /*!
     * \brief construct with default value
     */
    option() = default;

    /*!
     * \brief assign a value
     *
     * \param value value
     * \return this object
     */
    option& operator=(const value_type& value) {
        if (!option_type::validate(value)) {
            throw exception(error_info(error_code::invalid_config_value,
                "invalid value for " + option_type::name()));
        }
        value_ = value;
        return *this;
    }

    /*!
     * \brief get value
     *
     * \return value
     */
    const value_type& value() const noexcept { return value_; }

private:
    //! value
    value_type value_{option_type::default_value()};
};

}  // namespace config
}  // namespace mprpc
