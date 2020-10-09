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
 * \brief test of buffer class
 */
#include "mprpc/buffer.h"

#include <type_traits>

#include <catch2/catch.hpp>

TEST_CASE("mprpc::buffer") {
    constexpr std::size_t initial_capacity = 1024;

    SECTION("constant") {
        REQUIRE(mprpc::buffer::max_size() ==
            std::numeric_limits<std::size_t>::max());
    }

    SECTION("default constructor") {
        std::shared_ptr<mprpc::buffer> buf;
        REQUIRE_NOTHROW(buf = std::make_shared<mprpc::buffer>());

        REQUIRE(buf->data() != nullptr);
        REQUIRE(buf->size() == 0);
        REQUIRE(buf->capacity() == initial_capacity);

        REQUIRE_NOTHROW(buf.reset());
    }

    SECTION("constructor with size smaller than initial capacity") {
        constexpr std::size_t size = 1000;
        std::shared_ptr<mprpc::buffer> buf;
        REQUIRE_NOTHROW(buf = std::make_shared<mprpc::buffer>(size));

        REQUIRE(buf->data() != nullptr);
        REQUIRE(buf->size() == size);
        REQUIRE(buf->capacity() == initial_capacity);
    }

    SECTION("constructor with size larger than initial capacity") {
        constexpr std::size_t size = 5000;
        constexpr std::size_t capacity = 8192;
        std::shared_ptr<mprpc::buffer> buf;
        REQUIRE_NOTHROW(buf = std::make_shared<mprpc::buffer>(size));

        REQUIRE(buf->data() != nullptr);
        REQUIRE(buf->size() == size);
        REQUIRE(buf->capacity() == capacity);
    }

    SECTION("resize") {
        mprpc::buffer buf;

        constexpr std::size_t size1 = 10;
        constexpr char test_byte = char(37);
        buf.resize(size1);
        REQUIRE(buf.size() == size1);
        REQUIRE(buf.capacity() == initial_capacity);
        buf.data()[size1 - 1] = test_byte;

        constexpr std::size_t size2 = 10000;
        constexpr std::size_t capacity2 = 16384;
        buf.resize(size2);
        REQUIRE(buf.size() == size2);
        REQUIRE(buf.capacity() == capacity2);
        REQUIRE(buf.data()[size1 - 1] == test_byte);

        constexpr std::size_t size3 = 1000;
        buf.resize(size3);
        REQUIRE(buf.size() == size3);
        REQUIRE(buf.capacity() == capacity2);
        REQUIRE(buf.data()[size1 - 1] == test_byte);
    }

    SECTION("reserve") {
        mprpc::buffer buf;

        constexpr std::size_t required_capacity1 = 10000;
        constexpr std::size_t capacity1 = 16384;
        buf.reserve(required_capacity1);
        REQUIRE(buf.size() == 0);
        REQUIRE(buf.capacity() == capacity1);

        constexpr std::size_t required_capacity2 = 1000;
        buf.reserve(required_capacity2);
        REQUIRE(buf.size() == 0);
        REQUIRE(buf.capacity() == capacity1);
    }

    SECTION("consume") {
        constexpr std::size_t size1 = 128;
        mprpc::buffer buf{size1};
        REQUIRE(buf.size() == size1);
        for (std::size_t i = 0; i < size1; ++i) {
            buf.data()[i] = static_cast<char>(i);
        }
        constexpr std::size_t consumed_size = 37;
        constexpr std::size_t size2 = size1 - consumed_size;
        buf.consume(consumed_size);
        REQUIRE(buf.size() == size2);
        for (std::size_t i = 0; i < size2; ++i) {
            INFO("i = " << i);
            REQUIRE(buf.data()[i] == static_cast<char>(i + consumed_size));
        }
    }
}
