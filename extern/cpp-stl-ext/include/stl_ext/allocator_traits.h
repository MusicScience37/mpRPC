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
 * \brief declaration and implementation of allocator_traits class
 */
#pragma once

#include <memory>

namespace stl_ext {

/*!
 * \brief helper class to use allocators
 *
 * \tparam Allocator allocator type
 */
template <typename Allocator>
struct allocator_traits : public std::allocator_traits<Allocator> {
    using typename std::allocator_traits<Allocator>::allocator_type;
    using typename std::allocator_traits<Allocator>::pointer;

    /*!
     * \brief get allocator_traits for another type
     *
     * \tparam T another type
     */
    template <typename T>
    using rebind_traits = allocator_traits<
        typename std::allocator_traits<Allocator>::template rebind_alloc<T>>;

    /*!
     * \brief reallocate memory
     *
     * \param alloc allocater
     * \param size new size
     * \param old_ptr old pointer
     * \return new pointer
     */
    static pointer reallocate(
        allocator_type& alloc, std::size_t size, pointer old_ptr) {
        return alloc.reallocate(size, old_ptr);
    }
};

}  // namespace stl_ext
