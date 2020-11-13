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
 * \brief test of shared_binary class
 */
#include "mprpc/shared_binary.h"

#include <algorithm>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#define REQUIRE_NULL(PTR) REQUIRE(static_cast<const void*>(PTR) == nullptr)
#define REQUIRE_NONNULL(PTR) REQUIRE(static_cast<const void*>(PTR) != nullptr)

TEST_CASE("mprpc::shared_binary") {
    SECTION("check of noexcept") {
        using test_type = mprpc::shared_binary;
        STATIC_REQUIRE(std::is_nothrow_default_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_copy_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_copy_assignable<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_move_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_move_assignable<test_type>::value);
    }

    SECTION("default construct") {
        mprpc::shared_binary data;

        REQUIRE(!data.has_data());
        REQUIRE_NULL(data.data());
        REQUIRE(data.size() == 0);
        REQUIRE(data.use_count() == 0);
    }

    SECTION("construct with data") {
        // NOLINTNEXTLINE: for test
        const char test_data[] = "a\0c";
        constexpr std::size_t test_data_size = sizeof(test_data) - 1;
        // NOLINTNEXTLINE: for test
        const auto data = mprpc::shared_binary(test_data, test_data_size);

        REQUIRE(data.has_data());
        REQUIRE_NONNULL(data.data());
        REQUIRE(data.size() == test_data_size);
        // NOLINTNEXTLINE: for test
        REQUIRE(std::equal(test_data, test_data + test_data_size, data.data()));
        REQUIRE(data.use_count() == 1);
    }

    SECTION("copy empty data") {
        mprpc::shared_binary data;

        SECTION("with copy constructor") {
            // NOLINTNEXTLINE: for test
            mprpc::shared_binary copy_data(data);

            REQUIRE(!copy_data.has_data());
            REQUIRE_NULL(copy_data.data());
            REQUIRE(copy_data.size() == 0);
            REQUIRE(copy_data.use_count() == 0);

            REQUIRE(copy_data == data);
        }

        SECTION("with copy assignment operator") {
            mprpc::shared_binary copy_data("a", 1);
            REQUIRE(copy_data.has_data());

            copy_data = data;

            REQUIRE(!copy_data.has_data());
            REQUIRE_NULL(copy_data.data());
            REQUIRE(copy_data.size() == 0);
            REQUIRE(copy_data.use_count() == 0);

            REQUIRE(copy_data == data);
        }
    }

    SECTION("copy non-empty data") {
        // NOLINTNEXTLINE: for test
        const char test_data[] = "a\0c";
        constexpr std::size_t test_data_size = sizeof(test_data) - 1;
        // NOLINTNEXTLINE: for test
        const auto data = mprpc::shared_binary(test_data, test_data_size);
        REQUIRE(data.size() == test_data_size);
        REQUIRE(data.use_count() == 1);

        SECTION("with copy constructor") {
            // NOLINTNEXTLINE: for test
            mprpc::shared_binary copy_data(data);

            REQUIRE(copy_data.has_data());
            REQUIRE_NONNULL(copy_data.data());
            REQUIRE(copy_data.size() == test_data_size);
            REQUIRE(data.use_count() == 2);
            REQUIRE(copy_data.use_count() == 2);

            REQUIRE(copy_data == data);
        }

        SECTION("with copy assignment operator") {
            mprpc::shared_binary copy_data;
            REQUIRE(!copy_data.has_data());

            copy_data = data;

            REQUIRE(copy_data.has_data());
            REQUIRE_NONNULL(copy_data.data());
            REQUIRE(copy_data.size() == test_data_size);
            REQUIRE(data.use_count() == 2);
            REQUIRE(copy_data.use_count() == 2);

            REQUIRE(copy_data == data);
        }
    }

    SECTION("move empty data") {
        mprpc::shared_binary data;

        SECTION("with move constructor") {
            // NOLINTNEXTLINE: for test
            mprpc::shared_binary copy_data(std::move(data));

            REQUIRE(!copy_data.has_data());
            REQUIRE_NULL(copy_data.data());
            REQUIRE(copy_data.size() == 0);
            REQUIRE(copy_data.use_count() == 0);

            // NOLINTNEXTLINE: for test
            REQUIRE(copy_data == data);
        }

        SECTION("with move assignment operator") {
            mprpc::shared_binary copy_data("a", 1);
            REQUIRE(copy_data.has_data());

            // NOLINTNEXTLINE: false positive
            copy_data = std::move(data);

            REQUIRE(!copy_data.has_data());
            REQUIRE_NULL(copy_data.data());
            REQUIRE(copy_data.size() == 0);
            REQUIRE(copy_data.use_count() == 0);

            // NOLINTNEXTLINE: for test
            REQUIRE(copy_data == data);
        }
    }

    SECTION("move non-empty data") {
        // NOLINTNEXTLINE: for test
        const char test_data[] = "a\0c";
        constexpr std::size_t test_data_size = sizeof(test_data) - 1;
        // NOLINTNEXTLINE: for test
        const auto origin = mprpc::shared_binary(test_data, test_data_size);
        mprpc::shared_binary data = origin;
        REQUIRE(data.size() == test_data_size);
        REQUIRE(data.use_count() == 2);

        SECTION("with move constructor") {
            // NOLINTNEXTLINE: for test
            mprpc::shared_binary copy_data(std::move(data));

            REQUIRE(!data.has_data());       // NOLINT: for test
            REQUIRE_NULL(data.data());       // NOLINT: for test
            REQUIRE(data.size() == 0);       // NOLINT: for test
            REQUIRE(data.use_count() == 0);  // NOLINT: for test

            REQUIRE(copy_data.has_data());
            REQUIRE_NONNULL(copy_data.data());
            REQUIRE(copy_data.size() == test_data_size);
            REQUIRE(copy_data.use_count() == 2);

            REQUIRE(copy_data == origin);
        }

        SECTION("with move assignment operator") {
            mprpc::shared_binary copy_data;
            REQUIRE(!copy_data.has_data());

            copy_data = std::move(data);

            REQUIRE(!data.has_data());       // NOLINT: for test
            REQUIRE_NULL(data.data());       // NOLINT: for test
            REQUIRE(data.size() == 0);       // NOLINT: for test
            REQUIRE(data.use_count() == 0);  // NOLINT: for test

            REQUIRE(copy_data.has_data());
            REQUIRE_NONNULL(copy_data.data());
            REQUIRE(copy_data.size() == test_data_size);
            REQUIRE(copy_data.use_count() == 2);

            REQUIRE(copy_data == origin);
        }
    }

    SECTION("clear data") {
        auto data = mprpc::shared_binary("a", 1);

        REQUIRE(data.has_data());
        REQUIRE_NONNULL(data.data());
        REQUIRE(data.size() == 1);
        REQUIRE(data.use_count() == 1);

        data.reset();

        REQUIRE(!data.has_data());
        REQUIRE_NULL(data.data());
        REQUIRE(data.size() == 0);
        REQUIRE(data.use_count() == 0);
    }
}
