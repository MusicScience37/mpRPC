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
 * \brief declaration and implementation of address class
 */
#pragma once

#include <string>

namespace mprpc {
namespace transport {

/*!
 * \brief base class of addresses
 */
class address {
public:
    /*!
     * \brief get full address
     *
     * \return full address
     */
    virtual std::string full_address() const = 0;

    //! construct
    address() noexcept = default;

    //! destruct
    virtual ~address() = default;

    address(const address&) = delete;
    address(address&&) = delete;
    address& operator=(const address&) = delete;
    address& operator=(address&&) = delete;
};

}  // namespace transport
}  // namespace mprpc
