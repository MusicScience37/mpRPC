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
 * \brief declaration and implementation of safe_double_size function
 */
#pragma once

#include <cstdint>
#include <limits>

namespace stl_ext {
namespace impl {

/*!
 * \brief get double size safely
 *
 * \param size old size
 * \return new size
 */
inline std::size_t safe_double_size(std::size_t size) noexcept {
    constexpr std::size_t max_size = std::numeric_limits<std::size_t>::max();
    constexpr std::size_t ratio = 2;
    constexpr std::size_t max_doubled_size = max_size / ratio;
    if (size > max_doubled_size) {
        return max_size;
    }
    return size * ratio;
}

}  // namespace impl
}  // namespace stl_ext
