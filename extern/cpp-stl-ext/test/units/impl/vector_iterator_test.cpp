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
 * \brief test of vector_iterator class
 */
#include "stl_ext/impl/vector_iterator.h"

#include <string>
#include <type_traits>
#include <vector>

#include <catch2/catch_test_macros.hpp>

TEST_CASE(
    "stl_ext::impl::vector_iterator<std::vector<std::string>, std::string*, "
    "std::string&>") {
    using value_type = std::string;
    using vector_type = std::vector<value_type>;
    using reference = value_type&;
    using pointer = value_type*;
    using test_type =
        stl_ext::impl::vector_iterator<vector_type, pointer, reference>;

    SECTION("use of std::iterator_traits") {
        using traits = std::iterator_traits<test_type>;
        STATIC_REQUIRE(
            std::is_same<traits::difference_type, std::ptrdiff_t>::value);
        STATIC_REQUIRE(std::is_same<traits::value_type, value_type>::value);
        STATIC_REQUIRE(std::is_same<traits::pointer, pointer>::value);
        STATIC_REQUIRE(std::is_same<traits::reference, reference>::value);
        STATIC_REQUIRE(std::is_same<traits::iterator_category,
            std::random_access_iterator_tag>::value);
    }

    SECTION("basic functions") {
        STATIC_REQUIRE(std::is_nothrow_default_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_copy_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_copy_assignable<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_move_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_move_assignable<test_type>::value);
    }

    SECTION("check requirements") {
        const value_type val1 = "abc";
        const value_type val2 = "defg";
        const value_type val3 = "hijkl";
        vector_type data{val1, val2, val3};

        auto iter1 = test_type(data.data());
        auto iter2 = test_type(data.data() + 1);
        auto iter3 = test_type(data.data() + 2);
        auto iter4 = test_type(data.data() + 3);

        SECTION("LegacyIterator") {
            // https://en.cppreference.com/w/cpp/named_req/Iterator
            REQUIRE(*iter1 == val1);
            STATIC_REQUIRE(std::is_same<decltype(++iter1), test_type&>::value);
        }

        SECTION("LegacyInputIterator") {
            // https://en.cppreference.com/w/cpp/named_req/InputIterator
            REQUIRE(iter1 == iter1);
            REQUIRE(iter1 != iter2);
            STATIC_REQUIRE(std::is_same<decltype(*iter1), reference>::value);
            REQUIRE(iter1->size() == (*iter1).size());
            REQUIRE(*iter1++ == val1);
        }

        SECTION("LegacyOutputIterator") {
            // https://en.cppreference.com/w/cpp/named_req/OutputIterator
            *iter1 = "a";
            REQUIRE(*iter1 == "a");
            auto iter = iter1;
            *iter++ = "b";
            REQUIRE(*iter1 == "b");
        }

        SECTION("LegacyForwardIterator") {
            // https://en.cppreference.com/w/cpp/named_req/ForwardIterator
            REQUIRE(iter1 < iter2);
            REQUIRE(iter1 <= iter2);
            REQUIRE(iter1 <= iter1);
            REQUIRE(iter2 > iter1);
            REQUIRE(iter2 >= iter1);
            REQUIRE(iter2 >= iter2);
        }

        SECTION("LegacyBidirectionalIterator") {
            // https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator
            auto iter = iter2;
            REQUIRE(--iter == iter1);
            iter = iter2;
            REQUIRE(*iter-- == val2);
        }

        SECTION("LegacyRandomAccessIterator") {
            // https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator
            REQUIRE(iter2 + 2 == iter4);
            REQUIRE(2 + iter2 == iter4);
            REQUIRE(iter3 - 2 == iter1);
            REQUIRE(iter1 - iter2 == -1);
            REQUIRE(iter3[-2] == val1);
        }
    }
}

TEST_CASE(
    "stl_ext::impl::vector_iterator<std::vector<std::string>, const "
    "std::string*, const std::string&>") {
    using value_type = std::string;
    using vector_type = std::vector<value_type>;
    using reference = const value_type&;
    using pointer = const value_type*;
    using test_type =
        stl_ext::impl::vector_iterator<vector_type, pointer, reference>;

    SECTION("use of std::iterator_traits") {
        using traits = std::iterator_traits<test_type>;
        STATIC_REQUIRE(
            std::is_same<traits::difference_type, std::ptrdiff_t>::value);
        STATIC_REQUIRE(std::is_same<traits::value_type, value_type>::value);
        STATIC_REQUIRE(std::is_same<traits::pointer, pointer>::value);
        STATIC_REQUIRE(std::is_same<traits::reference, reference>::value);
        STATIC_REQUIRE(std::is_same<traits::iterator_category,
            std::random_access_iterator_tag>::value);
    }

    SECTION("basic functions") {
        STATIC_REQUIRE(std::is_nothrow_default_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_copy_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_copy_assignable<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_move_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_move_assignable<test_type>::value);
    }

    SECTION("check requirements") {
        const value_type val1 = "abc";
        const value_type val2 = "defg";
        const value_type val3 = "hijkl";
        vector_type data{val1, val2, val3};

        auto iter1 = test_type(data.data());
        auto iter2 = test_type(data.data() + 1);
        auto iter3 = test_type(data.data() + 2);
        auto iter4 = test_type(data.data() + 3);

        SECTION("LegacyIterator") {
            // https://en.cppreference.com/w/cpp/named_req/Iterator
            REQUIRE(*iter1 == val1);
            STATIC_REQUIRE(std::is_same<decltype(++iter1), test_type&>::value);
        }

        SECTION("LegacyInputIterator") {
            // https://en.cppreference.com/w/cpp/named_req/InputIterator
            REQUIRE(iter1 == iter1);
            REQUIRE(iter1 != iter2);
            STATIC_REQUIRE(std::is_same<decltype(*iter1), reference>::value);
            REQUIRE(iter1->size() == (*iter1).size());
            REQUIRE(*iter1++ == val1);
        }

        SECTION("LegacyForwardIterator") {
            // https://en.cppreference.com/w/cpp/named_req/ForwardIterator
            REQUIRE(iter1 < iter2);
            REQUIRE(iter1 <= iter2);
            REQUIRE(iter1 <= iter1);
            REQUIRE(iter2 > iter1);
            REQUIRE(iter2 >= iter1);
            REQUIRE(iter2 >= iter2);
        }

        SECTION("LegacyBidirectionalIterator") {
            // https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator
            auto iter = iter2;
            REQUIRE(--iter == iter1);
            iter = iter2;
            REQUIRE(*iter-- == val2);
        }

        SECTION("LegacyRandomAccessIterator") {
            // https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator
            REQUIRE(iter2 + 2 == iter4);
            REQUIRE(2 + iter2 == iter4);
            REQUIRE(iter3 - 2 == iter1);
            REQUIRE(iter1 - iter2 == -1);
            REQUIRE(iter3[-2] == val1);
        }
    }
}

TEST_CASE("stl_ext::impl::vector_iterator<std::vector<std::string>>") {
    SECTION("compatibility of non-const and const iterators") {
        std::vector<std::string> vec = {"abc", "def"};
        using iterator =
            stl_ext::impl::vector_iterator<std::vector<std::string>,
                std::string*, std::string&>;
        using const_iterator =
            stl_ext::impl::vector_iterator<std::vector<std::string>,
                const std::string*, const std::string&>;

        iterator iter1(vec.data());
        const_iterator iter2(iter1);
        REQUIRE(iter1 == iter2);
    }
}
