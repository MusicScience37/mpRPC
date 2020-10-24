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
 * \brief test of pod_vector class
 */
#include "stl_ext/pod_vector.h"

#include <vector>

#include <catch2/catch_test_macros.hpp>

// NOLINTNEXTLINE: test of large class
TEST_CASE("stl_ext::pod_vector<int>") {
    using value_type = int;
    using test_type = stl_ext::pod_vector<value_type>;

    SECTION("default constructor") {
        test_type vec;
        REQUIRE(static_cast<void*>(vec.data()) != nullptr);
        REQUIRE(vec.size() == 0);      // NOLINT: for test
        REQUIRE(vec.capacity() == 8);  // NOLINT: for test
        REQUIRE(vec.empty());
    }

    SECTION("constructor with size") {
        constexpr std::size_t size1 = 4;
        constexpr std::size_t capacity1 = 8;
        test_type vec1(size1);
        REQUIRE(static_cast<void*>(vec1.data()) != nullptr);
        REQUIRE(vec1.size() == size1);
        REQUIRE(vec1.capacity() == capacity1);
        REQUIRE_FALSE(vec1.empty());

        constexpr std::size_t size2 = 100;
        constexpr std::size_t capacity2 = 128;
        test_type vec2(size2);
        REQUIRE(static_cast<void*>(vec2.data()) != nullptr);
        REQUIRE(vec2.size() == size2);
        REQUIRE(vec2.capacity() == capacity2);
        REQUIRE_FALSE(vec2.empty());
    }

    SECTION("access to data") {
        constexpr std::size_t size = 11;
        test_type vec(size);
        for (std::size_t i = 0; i < size; ++i) {
            REQUIRE_NOTHROW(vec[i] = static_cast<value_type>(i));
        }
        const auto& const_vec = vec;
        for (std::size_t i = 0; i < size; ++i) {
            REQUIRE(const_vec[i] == static_cast<value_type>(i));
        }
    }

    SECTION("copy data") {
        const std::vector<value_type> data = {3, 5, 7, 11};
        test_type vec(data.begin(), data.size());
        REQUIRE(vec.size() == data.size());
        REQUIRE(vec.capacity() >= vec.size());
        for (std::size_t i = 0; i < data.size(); ++i) {
            REQUIRE(vec[i] == data[i]);
        }
    }

    SECTION("construct from initializer list") {
        constexpr value_type value1 = 5;
        constexpr value_type value2 = 8;
        constexpr value_type value3 = 37;
        constexpr value_type value4 = 1;
        constexpr std::size_t size = 4;
        test_type vec{value1, value2, value3, value4};
        REQUIRE(vec.size() == size);
        REQUIRE(vec.capacity() >= vec.size());
        REQUIRE(vec[0] == value1);
        REQUIRE(vec[1] == value2);
        REQUIRE(vec[2] == value3);
        REQUIRE(vec[3] == value4);
    }

    SECTION("copy") {
        const test_type data1{9, 4, 7};
        test_type vec(data1);
        REQUIRE(vec.size() == data1.size());
        REQUIRE(vec.capacity() == data1.capacity());
        for (std::size_t i = 0; i < vec.size(); ++i) {
            REQUIRE(vec[i] == data1[i]);
        }

        const test_type data2{37, 123, 4, 6, 918};
        vec = data2;
        REQUIRE(vec.size() == data2.size());
        REQUIRE(vec.capacity() == data2.capacity());
        for (std::size_t i = 0; i < vec.size(); ++i) {
            REQUIRE(vec[i] == data2[i]);
        }

        // NOLINTNEXTLINE: test of self assignment
        vec = vec;
        REQUIRE(vec.size() == data2.size());
        REQUIRE(vec.capacity() == data2.capacity());
        for (std::size_t i = 0; i < vec.size(); ++i) {
            REQUIRE(vec[i] == data2[i]);
        }
    }

    SECTION("move") {
        const test_type data1{9, 4, 7};
        test_type vec1 = data1;

        test_type vec2(std::move(vec1));
        REQUIRE(vec2.size() == data1.size());
        REQUIRE(vec2.capacity() == data1.capacity());
        for (std::size_t i = 0; i < vec2.size(); ++i) {
            REQUIRE(vec2[i] == data1[i]);
        }

        const test_type data2{4, 7};
        vec1 = data2;

        vec2 = std::move(vec1);
        REQUIRE(vec2.size() == data2.size());
        REQUIRE(vec2.capacity() == data2.capacity());
        for (std::size_t i = 0; i < vec2.size(); ++i) {
            REQUIRE(vec2[i] == data2[i]);
        }
    }

    SECTION("access to data with boundary check") {
        constexpr std::size_t size = 5;
        test_type vec;
        vec.resize(size);

        constexpr value_type val = 37;
        REQUIRE_NOTHROW(vec.at(size - 1) = val);
        REQUIRE_THROWS(vec.at(size) = val);

        const auto& const_vec = vec;
        REQUIRE_NOTHROW(const_vec.at(size - 1));
        REQUIRE_THROWS(const_vec.at(size));
    }

    SECTION("get iterator") {
        constexpr value_type value1 = 7;
        constexpr value_type value2 = 5;
        constexpr value_type value3 = 1;
        test_type vec{value1, value2, value3};

        SECTION("non-const iterator") {
            auto iter = vec.begin();
            REQUIRE(*iter == value1);
            ++iter;
            REQUIRE(*iter == value2);
            ++iter;
            REQUIRE(*iter == value3);
            ++iter;
            REQUIRE(iter == vec.end());
        }

        SECTION("const iterator from const object") {
            const auto& const_vec = vec;
            auto iter = const_vec.begin();
            REQUIRE(*iter == value1);
            ++iter;
            REQUIRE(*iter == value2);
            ++iter;
            REQUIRE(*iter == value3);
            ++iter;
            REQUIRE(iter == const_vec.end());
        }

        SECTION("const iterator from non-const object") {
            auto iter = vec.cbegin();
            REQUIRE(*iter == value1);
            ++iter;
            REQUIRE(*iter == value2);
            ++iter;
            REQUIRE(*iter == value3);
            ++iter;
            REQUIRE(iter == vec.cend());
        }
    }

    SECTION("insert a range") {
        constexpr value_type value1 = 7;
        constexpr value_type value2 = 5;
        constexpr value_type value3 = 1;
        test_type vec{value1, value2, value3};

        const test_type data{37, 123};
        auto iter = vec.insert(vec.cbegin() + 1, data.cbegin(), data.cend());
        REQUIRE(*iter == data.at(0));
        REQUIRE(vec.size() == 5);
        REQUIRE(vec.at(0) == value1);
        REQUIRE(vec.at(1) == data.at(0));
        REQUIRE(vec.at(2) == data.at(1));
        REQUIRE(vec.at(3) == value2);
        REQUIRE(vec.at(4) == value3);
    }

    SECTION("insert a value") {
        constexpr value_type value1 = 7;
        constexpr value_type value2 = 5;
        constexpr value_type value3 = 1;
        test_type vec{value1, value2, value3};

        constexpr value_type data = 37;
        auto iter = vec.insert(vec.cbegin() + 1, data);
        REQUIRE(*iter == data);
        REQUIRE(vec.size() == 4);
        REQUIRE(vec.at(0) == value1);
        REQUIRE(vec.at(1) == data);
        REQUIRE(vec.at(2) == value2);
        REQUIRE(vec.at(3) == value3);
    }

    SECTION("insert a value to the end") {
        constexpr value_type value1 = 7;
        constexpr value_type value2 = 5;
        constexpr value_type value3 = 1;
        test_type vec{value1, value2, value3};

        constexpr value_type data = 37;
        vec.push_back(data);
        REQUIRE(vec.size() == 4);
        REQUIRE(vec.at(0) == value1);
        REQUIRE(vec.at(1) == value2);
        REQUIRE(vec.at(2) == value3);
        REQUIRE(vec.at(3) == data);
    }

    SECTION("erase a range") {
        constexpr value_type value1 = 7;
        constexpr value_type value2 = 5;
        constexpr value_type value3 = 1;
        test_type vec{value1, value2, value3};

        auto iter = vec.erase(vec.cbegin(), vec.cbegin() + 2);
        REQUIRE(vec.size() == 1);
        REQUIRE(vec.at(0) == value3);
        REQUIRE(*iter == value3);
    }

    SECTION("erase an element") {
        constexpr value_type value1 = 7;
        constexpr value_type value2 = 5;
        constexpr value_type value3 = 1;
        test_type vec{value1, value2, value3};

        auto iter = vec.erase(vec.cbegin() + 1);
        REQUIRE(vec.size() == 2);
        REQUIRE(vec.at(1) == value3);
        REQUIRE(*iter == value3);
    }

    SECTION("compare vectors") {
        const test_type vec1 = {1, 2, 3, 4, 5};
        const test_type vec2 = {1, 2, 3, 4, 5};
        const test_type vec3 = {1, 2, 3, 4, 4};
        const test_type vec4 = {1, 2, 3, 4, 5, 6};

        REQUIRE(vec1 == vec2);
        REQUIRE(vec1 != vec3);
        REQUIRE(vec1 != vec4);
    }
}
