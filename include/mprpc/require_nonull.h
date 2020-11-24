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
 * \brief declaration and implementation of require_nonull function
 */
#pragma once

#include <string>

#include "mprpc/error_code.h"
#include "mprpc/exception.h"
#include "position_macros.h"

namespace mprpc {

/*!
 * \brief check ptr is not null, and throw exception if not
 *
 * \tparam Pointer pointer type
 * \param ptr pointer
 * \param name name
 * \param where position in source codes
 * \return
 */
template <typename Pointer>
Pointer require_nonull(
    const Pointer& ptr, const char* name, const char* where) {
    if (name == nullptr) {
        throw exception(error_info(error_code::unexpected_nullptr,
            "unexpected null pointer for name at mprpc::require_nonull"));
    }
    if (where == nullptr) {
        throw exception(error_info(error_code::unexpected_nullptr,
            "unexpected null pointer for where at mprpc::require_nonull"));
    }
    if (ptr == nullptr) {
        throw exception(error_info(error_code::unexpected_nullptr,
            std::string() + "unexpected null pointer for " + name + " at " +
                where));
    }
    return ptr;
}

}  // namespace mprpc

/*!
 * \brief check POINTER is not null, and throw exception if not
 *
 * \param POINTER pointer
 */
#define MPRPC_REQUIRE_NONULL(POINTER)                                    \
    ::mprpc::require_nonull(POINTER, static_cast<const char*>(#POINTER), \
        static_cast<const char*>(MPRPC_FUNCTION))
