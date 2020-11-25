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
 * \brief declaration of bind_logging function
 */
#include "mprpc/format_bytes.h"

#include <fmt/core.h>

#include "mprpc/require_nonull.h"

namespace mprpc {
namespace python {

std::string format_bytes(const char* data, std::size_t size) {
    MPRPC_REQUIRE_NONULL(data);
    std::string result;
    result += 'b';
    result += '\'';
    for (std::size_t i = 0; i < size; ++i) {
        result += fmt::format("\\x{:02x}", static_cast<unsigned char>(data[i]));
    }
    result += '\'';
    return result;
}

}  // namespace python
}  // namespace mprpc
