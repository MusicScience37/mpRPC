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
 * \brief declaration and implementation of pod_allocator class
 */
#pragma once

#include <cstddef>
#include <cstdlib>
#include <new>
#include <type_traits>

#include "stl_ext/ignore.h"

namespace stl_ext {

/*!
 * \brief class of allocators for POD objects
 *
 * \tparam ValueType value type
 */
template <typename ValueType, typename = void>
class pod_allocator;

/*!
 * \brief pod_allocator implementation
 *
 * \tparam ValueType value type
 */
template <typename ValueType>
class pod_allocator<ValueType,
    std::enable_if_t<std::is_trivially_copyable<ValueType>::value &&
        alignof(ValueType) <= alignof(std::max_align_t)>> {
public:
    //! value type
    using value_type = ValueType;

    //! size type
    using size_type = std::size_t;

    //! type of pointer differences
    using difference_type = std::ptrdiff_t;

    //! copy allocator object on move assignment
    using propagate_on_container_move_assignment = std::true_type;

    //! whether objects of this class is always equals
    using is_always_equal = std::true_type;

    /*!
     * \brief allocate memory
     *
     * \param size size
     * \return pointer
     */
    value_type* allocate(std::size_t size) {
        // NOLINTNEXTLINE: this is allocator
        void* ptr = std::malloc(sizeof(value_type) * size);
        if (ptr == nullptr) {
            throw std::bad_alloc();
        }
        return static_cast<value_type*>(ptr);
    }

    /*!
     * \brief reallocate memory
     *
     * \param size new size
     * \param old_ptr old pointer
     * \return new pointer
     */
    value_type* reallocate(std::size_t size, value_type* old_ptr) {
        // NOLINTNEXTLINE: this is allocator
        void* ptr = std::realloc(old_ptr, sizeof(value_type) * size);
        if (ptr == nullptr) {
            throw std::bad_alloc();
        }
        return static_cast<value_type*>(ptr);
    }

    /*!
     * \brief free memory
     *
     * \param ptr pointer
     * \param size size
     */
    void deallocate(value_type* ptr, std::size_t size) noexcept {
        ignore(size);
        // NOLINTNEXTLINE: this is allocator
        std::free(ptr);
    }
};

}  // namespace stl_ext
