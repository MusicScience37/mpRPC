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
 * \brief test of shared_function class
 */
#include "stl_ext/shared_function.h"

#include <string>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("stl_ext::shared_function") {
    SECTION("basic functions") {
        using test_type = stl_ext::shared_function<std::string(int, int)>;
        STATIC_REQUIRE(std::is_nothrow_default_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_copy_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_copy_assignable<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_move_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_move_assignable<test_type>::value);
    }

    SECTION("null function") {
        const stl_ext::shared_function<std::string()> func;
        REQUIRE(!func);
        REQUIRE_THROWS_AS(func(), stl_ext::bad_shared_function_call);
    }

    SECTION("null function with nullptr") {
        stl_ext::shared_function<std::string()> func{nullptr};
        REQUIRE(!func);
        REQUIRE_THROWS_AS(func(), stl_ext::bad_shared_function_call);

        func = nullptr;
        REQUIRE(!func);
    }

    SECTION("simple function") {
        const auto origin = [] { return std::string("abc"); };
        stl_ext::shared_function<std::string()> func = origin;
        REQUIRE(func);
        REQUIRE(func() == "abc");

        func = nullptr;
        REQUIRE(!func);

        func = origin;
        REQUIRE(func);
        REQUIRE(func() == "abc");
    }

    SECTION("function with arguments") {
        const auto origin = [](int x, int y) { return x + y; };
        stl_ext::shared_function<int(int, int)> func = origin;
        REQUIRE(func);
        REQUIRE(func(1, 2) == 3);

        func = nullptr;
        REQUIRE(!func);

        func = origin;
        REQUIRE(func);
        REQUIRE(func(1, 2) == 3);
    }

    SECTION("heavy function") {
        const auto data = std::string(1024, 'a');
        // NOLINTNEXTLINE: test of copy
        const auto origin = [data] { return data; };
        stl_ext::shared_function<std::string()> func = origin;
        REQUIRE(func);
        REQUIRE(func() == data);

        func = nullptr;
        REQUIRE(!func);

        func = origin;
        REQUIRE(func);
        REQUIRE(func() == data);
    }

    SECTION("copy") {
        const auto origin = [](int x, int y) { return x + y; };
        stl_ext::shared_function<int(int, int)> func = origin;
        REQUIRE(func);
        REQUIRE(func(1, 2) == 3);

        auto copy = func;
        REQUIRE(copy);
        REQUIRE(copy(1, 2) == 3);

        copy = func;
        REQUIRE(copy);
        REQUIRE(copy(1, 2) == 3);
    }

    SECTION("move") {
        const auto origin = [](int x, int y) { return x + y; };
        stl_ext::shared_function<int(int, int)> func = origin;
        REQUIRE(func);
        REQUIRE(func(1, 2) == 3);

        auto copy = std::move(func);
        REQUIRE(copy);
        REQUIRE(copy(1, 2) == 3);

        func = origin;
        copy = std::move(func);
        REQUIRE(copy);
        REQUIRE(copy(1, 2) == 3);
    }
}
