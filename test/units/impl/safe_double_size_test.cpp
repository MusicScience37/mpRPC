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
 * \brief test of safe_double_size function
 */
#include "stl_ext/impl/safe_double_size.h"

#include <limits>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("stl_ext::impl::safe_double_size") {
    using stl_ext::impl::safe_double_size;

    SECTION("ordinary values") {
        REQUIRE(safe_double_size(3) == 6);
        REQUIRE(safe_double_size(5) == 10);
    }

    SECTION("boundary values") {
        static constexpr auto max_size =
            std::numeric_limits<std::size_t>::max();

        REQUIRE(safe_double_size(max_size / 2) == max_size - 1);
        REQUIRE(safe_double_size(max_size / 2 + 1) == max_size);
    }
}
