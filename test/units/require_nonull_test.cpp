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
 * \brief test of require_nonull function
 */
#include "mprpc/require_nonull.h"

#include <memory>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "catch2/matchers/catch_matchers.hpp"

TEST_CASE("mprpc::require_nonull") {
    using mprpc::require_nonull;
    using mprpc::require_nonull_move;

    const std::string where_str = MPRPC_FILENAME;
    const char* where = where_str.c_str();

    SECTION("non-null pointers") {
        const char* char_ptr = "test";
        REQUIRE(require_nonull(char_ptr, "char_ptr", where) == char_ptr);

        const auto int_shared_ptr = std::make_shared<int>(0);
        REQUIRE(require_nonull(int_shared_ptr, "int_shared_ptr", where) ==
            int_shared_ptr);

        constexpr int int_val = 37;
        auto int_unique_ptr = std::make_unique<int>(int_val);
        std::unique_ptr<int> moved_int_unique_ptr;
        REQUIRE_NOTHROW(
            moved_int_unique_ptr = require_nonull_move(
                std::move(int_unique_ptr), "int_unique_ptr", where));
        REQUIRE(*moved_int_unique_ptr == int_val);
    }

    SECTION("null pointer") {
        const char* char_ptr = nullptr;
        REQUIRE_THROWS_WITH(require_nonull(char_ptr, "char_ptr", where),
            Catch::Matchers::Contains("char_ptr") &&
                Catch::Matchers::Contains(where));

        const std::shared_ptr<int> int_shared_ptr;
        REQUIRE_THROWS_WITH(
            require_nonull(int_shared_ptr, "int_shared_ptr", where),
            Catch::Matchers::Contains("int_shared_ptr") &&
                Catch::Matchers::Contains(where));

        std::unique_ptr<int> int_unique_ptr;
        REQUIRE_THROWS_WITH(require_nonull_move(std::move(int_unique_ptr),
                                "int_unique_ptr", where),
            Catch::Matchers::Contains("int_unique_ptr") &&
                Catch::Matchers::Contains(where));
    }

    SECTION("null pointer for name") {
        const char* char_ptr = "test";
        REQUIRE_THROWS_WITH(require_nonull(char_ptr, nullptr, where),
            Catch::Matchers::Contains("name") &&
                Catch::Matchers::Contains("mprpc::require_nonull"));
    }

    SECTION("null pointer for where") {
        const char* char_ptr = "test";
        REQUIRE_THROWS_WITH(require_nonull(char_ptr, "char_test", nullptr),
            Catch::Matchers::Contains("where") &&
                Catch::Matchers::Contains("mprpc::require_nonull"));
    }
}

namespace {

template <typename Pointer>
Pointer copy_ptr(const Pointer& origin) {
    return MPRPC_REQUIRE_NONULL(origin);
}

template <typename Pointer>
Pointer move_ptr(Pointer origin) {
    return MPRPC_REQUIRE_NONULL_MOVE(origin);
}

}  // namespace

TEST_CASE("MPRPC_REQUIRE_NONULL") {
    SECTION("non-null pointers") {
        const char* char_ptr = "test";
        REQUIRE(copy_ptr(char_ptr) == char_ptr);

        const std::shared_ptr<int> int_shared_ptr = std::make_shared<int>(0);
        REQUIRE(copy_ptr(int_shared_ptr) == int_shared_ptr);

        constexpr int int_val = 37;
        auto int_unique_ptr = std::make_unique<int>(int_val);
        std::unique_ptr<int> moved_int_unique_ptr;
        REQUIRE_NOTHROW(
            moved_int_unique_ptr = move_ptr(std::move(int_unique_ptr)));
    }

    SECTION("null pointers") {
        const char* char_ptr = nullptr;
        REQUIRE_THROWS_WITH(copy_ptr(char_ptr),
            Catch::Matchers::Contains("origin") &&
                Catch::Matchers::Contains("copy_ptr"));

        const std::shared_ptr<int> int_shared_ptr;
        REQUIRE_THROWS_WITH(copy_ptr(int_shared_ptr),
            Catch::Matchers::Contains("origin") &&
                Catch::Matchers::Contains("copy_ptr"));

        std::unique_ptr<int> int_unique_ptr;
        REQUIRE_THROWS_WITH(move_ptr(std::move(int_unique_ptr)),
            Catch::Matchers::Contains("origin") &&
                Catch::Matchers::Contains("move_ptr"));
    }
}
