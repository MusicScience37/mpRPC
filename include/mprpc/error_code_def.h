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
 * \brief basic definition of error codes
 */
#pragma once

#include <cstdint>

namespace mprpc {

/*!
 * \brief type of error codes
 */
using error_code_t = std::uint32_t;

/*!
 * \brief namespace of error codes
 */
namespace error_code {

/*!
 * \brief enumeration of common error codes
 */
enum common_errors : error_code_t {
    success = 0,      //!< success (no error)
    unexpected_error  //!< unexpected error
};

}  // namespace error_code

}  // namespace mprpc
