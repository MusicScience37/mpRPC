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
 * \brief test of allocator_traits class
 */
#include "stl_ext/allocator_traits.h"

#include <limits>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("stl_ext::allocator_traits<std::allocator<int>>") {
    using value_type = int;
    using allocator_type = std::allocator<value_type>;
    using traits_type = stl_ext::allocator_traits<allocator_type>;

    SECTION("typedefs") {
        STATIC_REQUIRE(std::is_same<typename traits_type::allocator_type,
            allocator_type>::value);
        STATIC_REQUIRE(
            std::is_same<typename traits_type::value_type, value_type>::value);
        STATIC_REQUIRE(
            std::is_same<typename traits_type::pointer, value_type*>::value);
        STATIC_REQUIRE(std::is_same<typename traits_type::const_pointer,
            const value_type*>::value);
        STATIC_REQUIRE(
            std::is_same<typename traits_type::void_pointer, void*>::value);
        STATIC_REQUIRE(std::is_same<typename traits_type::const_void_pointer,
            const void*>::value);
        STATIC_REQUIRE(std::is_same<typename traits_type::difference_type,
            std::ptrdiff_t>::value);
        STATIC_REQUIRE(
            std::is_same<typename traits_type::size_type, std::size_t>::value);
        STATIC_REQUIRE_FALSE(
            traits_type::propagate_on_container_copy_assignment::value);
        STATIC_REQUIRE(
            traits_type::propagate_on_container_move_assignment::value);
        STATIC_REQUIRE_FALSE(traits_type::propagate_on_container_swap::value);
    }

    SECTION("templates") {
        STATIC_REQUIRE(std::is_same<typename traits_type::rebind_alloc<float>,
            std::allocator<float>>::value);
        STATIC_REQUIRE(std::is_same<typename traits_type::rebind_traits<float>,
            stl_ext::allocator_traits<std::allocator<float>>>::value);
    }

    SECTION("max size") {
        allocator_type alloc;
        REQUIRE_NOTHROW(traits_type::max_size(alloc));
    }

    SECTION("use memory") {
        constexpr std::size_t size = 5;
        allocator_type alloc;

        auto* ptr = traits_type::allocate(alloc, size);
        REQUIRE(static_cast<void*>(ptr) != nullptr);

        constexpr std::size_t ind = 3;
        const auto value = value_type();
        traits_type::construct(alloc, ptr + ind, value);
        REQUIRE(*(ptr + ind) == value);

        traits_type::destroy(alloc, ptr + ind);

        traits_type::deallocate(alloc, ptr, size);
    }
}
