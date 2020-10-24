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
 * \brief test of shared_any class
 */
#include "stl_ext/shared_any.h"

#include <memory>
#include <string>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("stl_ext::impl::shared_any_impl<int>") {
    using test_type = int;
    using impl_type = stl_ext::impl::shared_any_impl<test_type>;
    using stl_ext::impl::shared_any_impl_operation;

    constexpr test_type test_data = 37;

    SECTION("create and destroy") {
        stl_ext::impl::shared_any_data data;

        impl_type::create(data, test_data);
        REQUIRE(data.obj_ptr != nullptr);
        REQUIRE(data.mng_ptr != nullptr);
        REQUIRE(*static_cast<test_type*>(data.obj_ptr) == test_data);
        REQUIRE(static_cast<typename impl_type::manager_data*>(data.mng_ptr)
                    ->ref_count.load() == 1);

        impl_type::destroy(data);
        REQUIRE(data.obj_ptr == nullptr);
        REQUIRE(data.mng_ptr == nullptr);
    }

    SECTION("get pointer") {
        stl_ext::impl::shared_any_data data;

        impl_type::create(data, test_data);

        const auto* ptr = impl_type::manage(
            shared_any_impl_operation::get_ptr, data, data, nullptr);
        REQUIRE(*static_cast<const test_type*>(ptr) == test_data);

        impl_type::destroy(data);
    }

    SECTION("get type") {
        stl_ext::impl::shared_any_data data;

        impl_type::create(data, test_data);

        const auto* ptr = impl_type::manage(
            shared_any_impl_operation::get_type, data, data, nullptr);
        REQUIRE(*static_cast<const std::type_info*>(ptr) == typeid(test_type));

        impl_type::destroy(data);
    }

    SECTION("add reference count") {
        stl_ext::impl::shared_any_data data;

        impl_type::create(data, test_data);
        auto* mng_ptr =
            static_cast<typename impl_type::manager_data*>(data.mng_ptr);
        REQUIRE(mng_ptr->ref_count.load() == 1);

        impl_type::manage(
            shared_any_impl_operation::add_ref, data, data, nullptr);
        REQUIRE(mng_ptr->ref_count.load() == 2);

        impl_type::destroy(data);
    }

    SECTION("release pointer") {
        stl_ext::impl::shared_any_data data;

        impl_type::create(data, test_data);
        auto* mng_ptr =
            static_cast<typename impl_type::manager_data*>(data.mng_ptr);
        mng_ptr->ref_count = 2;

        impl_type::manage(
            shared_any_impl_operation::release, data, data, nullptr);
        REQUIRE(data.obj_ptr != nullptr);
        REQUIRE(data.mng_ptr != nullptr);
        REQUIRE(mng_ptr->ref_count.load() == 1);

        impl_type::manage(
            shared_any_impl_operation::release, data, data, nullptr);
        REQUIRE(data.obj_ptr == nullptr);
        REQUIRE(data.mng_ptr == nullptr);

        impl_type::destroy(data);
    }

    SECTION("get reference count") {
        stl_ext::impl::shared_any_data data;

        impl_type::create(data, test_data);
        auto* mng_ptr =
            static_cast<typename impl_type::manager_data*>(data.mng_ptr);
        constexpr unsigned int test_val = 5;
        mng_ptr->ref_count = test_val;

        unsigned int ref_count = 0;
        impl_type::manage(
            shared_any_impl_operation::get_ref_count, data, data, &ref_count);
        REQUIRE(ref_count == test_val);

        impl_type::destroy(data);
    }
}

TEST_CASE("stl_ext::impl::shared_any_impl<std::string>") {
    using test_type = std::string;
    using impl_type = stl_ext::impl::shared_any_impl<std::string>;
    using stl_ext::impl::shared_any_impl_operation;

    const auto test_data = std::string("abc");

    SECTION("create and destroy") {
        stl_ext::impl::shared_any_data data;

        impl_type::create(data, test_data);
        REQUIRE(data.obj_ptr != nullptr);
        REQUIRE(data.mng_ptr != nullptr);
        REQUIRE(*static_cast<test_type*>(data.obj_ptr) == test_data);
        REQUIRE(static_cast<typename impl_type::manager_data*>(data.mng_ptr)
                    ->ref_count.load() == 1);

        impl_type::destroy(data);
        REQUIRE(data.obj_ptr == nullptr);
        REQUIRE(data.mng_ptr == nullptr);
    }

    SECTION("get pointer") {
        stl_ext::impl::shared_any_data data;

        impl_type::create(data, test_data);

        const auto* ptr = impl_type::manage(
            shared_any_impl_operation::get_ptr, data, data, nullptr);
        REQUIRE(*static_cast<const test_type*>(ptr) == test_data);

        impl_type::destroy(data);
    }

    SECTION("get type") {
        stl_ext::impl::shared_any_data data;

        impl_type::create(data, test_data);

        const auto* ptr = impl_type::manage(
            shared_any_impl_operation::get_type, data, data, nullptr);
        REQUIRE(*static_cast<const std::type_info*>(ptr) == typeid(test_type));

        impl_type::destroy(data);
    }

    SECTION("add reference count") {
        stl_ext::impl::shared_any_data data;

        impl_type::create(data, test_data);
        auto* mng_ptr =
            static_cast<typename impl_type::manager_data*>(data.mng_ptr);
        REQUIRE(mng_ptr->ref_count.load() == 1);

        impl_type::manage(
            shared_any_impl_operation::add_ref, data, data, nullptr);
        REQUIRE(mng_ptr->ref_count.load() == 2);

        impl_type::destroy(data);
    }

    SECTION("release pointer") {
        stl_ext::impl::shared_any_data data;

        impl_type::create(data, test_data);
        auto* mng_ptr =
            static_cast<typename impl_type::manager_data*>(data.mng_ptr);
        mng_ptr->ref_count = 2;

        impl_type::manage(
            shared_any_impl_operation::release, data, data, nullptr);
        REQUIRE(data.obj_ptr != nullptr);
        REQUIRE(data.mng_ptr != nullptr);
        REQUIRE(mng_ptr->ref_count.load() == 1);

        impl_type::manage(
            shared_any_impl_operation::release, data, data, nullptr);
        REQUIRE(data.obj_ptr == nullptr);
        REQUIRE(data.mng_ptr == nullptr);

        impl_type::destroy(data);
    }

    SECTION("get reference count") {
        stl_ext::impl::shared_any_data data;

        impl_type::create(data, test_data);
        auto* mng_ptr =
            static_cast<typename impl_type::manager_data*>(data.mng_ptr);
        constexpr unsigned int test_val = 5;
        mng_ptr->ref_count = test_val;

        unsigned int ref_count = 0;
        impl_type::manage(
            shared_any_impl_operation::get_ref_count, data, data, &ref_count);
        REQUIRE(ref_count == test_val);

        impl_type::destroy(data);
    }
}

TEST_CASE("stl_ext::shared_any") {
    SECTION("basic functions") {
        using test_type = stl_ext::shared_any;
        STATIC_REQUIRE(std::is_nothrow_default_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_copy_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_copy_assignable<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_move_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_move_assignable<test_type>::value);
    }

    SECTION("create and destroy null value") {
        auto any_ptr = std::make_unique<stl_ext::shared_any>();
        REQUIRE(any_ptr->has_value() == false);
        REQUIRE(any_ptr->type() == typeid(stl_ext::null_shared_any_t));
        REQUIRE(any_ptr->void_ptr() == nullptr);
        REQUIRE_THROWS_AS(any_ptr->get<int>(), stl_ext::bad_shared_any_cast);
        REQUIRE(any_ptr->use_count() == 0);
        any_ptr->reset();
    }

    SECTION("copy null value") {
        stl_ext::shared_any any;
        REQUIRE(any.has_value() == false);
        REQUIRE(any.use_count() == 0);

        // NOLINTNEXTLINE: test of copy
        stl_ext::shared_any copy(any);
        REQUIRE(any.has_value() == false);
        REQUIRE(any.use_count() == 0);
        REQUIRE(copy.has_value() == false);
        REQUIRE(copy.use_count() == 0);

        copy = any;
        REQUIRE(any.has_value() == false);
        REQUIRE(any.use_count() == 0);
        REQUIRE(copy.has_value() == false);
        REQUIRE(copy.use_count() == 0);

        // NOLINTNEXTLINE: test of copy
        any = any;
        REQUIRE(any.has_value() == false);
        REQUIRE(any.use_count() == 0);
    }

    SECTION("create and destroy int value") {
        constexpr int test_data = 37;
        auto any_ptr = std::make_unique<stl_ext::shared_any>(test_data);
        REQUIRE(any_ptr->has_value() == true);
        REQUIRE(any_ptr->type() == typeid(int));
        REQUIRE(any_ptr->get<int>() == test_data);
        REQUIRE_THROWS_AS(any_ptr->get<long>(), stl_ext::bad_shared_any_cast);
        REQUIRE(any_ptr->use_count() == 1);
        any_ptr->reset();
    }

    SECTION("copy int") {
        constexpr int test_data = 37;
        stl_ext::shared_any any = test_data;
        REQUIRE(any.get<int>() == test_data);
        REQUIRE(any.use_count() == 1);

        // NOLINTNEXTLINE: test of copy
        stl_ext::shared_any copy(any);
        REQUIRE(any.get<int>() == test_data);
        REQUIRE(copy.has_value() == true);
        REQUIRE(copy.get<int>() == test_data);
        REQUIRE(any.use_count() == 2);
        REQUIRE(copy.use_count() == 2);

        copy.reset();
        REQUIRE(copy.has_value() == false);
        REQUIRE(any.use_count() == 1);
        REQUIRE(copy.use_count() == 0);

        copy = any;
        REQUIRE(copy.has_value() == true);
        REQUIRE(copy.get<int>() == test_data);
        REQUIRE(any.use_count() == 2);
        REQUIRE(copy.use_count() == 2);

        any.reset();
        REQUIRE(any.use_count() == 0);
        REQUIRE(any.void_ptr() == nullptr);
        REQUIRE(copy.use_count() == 1);

        copy.reset();
        REQUIRE(copy.use_count() == 0);
        REQUIRE(copy.void_ptr() == nullptr);
    }

    SECTION("create and destroy std::string value") {
        const auto test_data = std::string("abc");
        auto any_ptr = std::make_unique<stl_ext::shared_any>(test_data);
        REQUIRE(any_ptr->has_value() == true);
        REQUIRE(any_ptr->type() == typeid(std::string));
        REQUIRE(any_ptr->get<std::string>() == test_data);
        REQUIRE_THROWS_AS(any_ptr->get<long>(), stl_ext::bad_shared_any_cast);
        REQUIRE(any_ptr->use_count() == 1);
        any_ptr->reset();

        REQUIRE(any_ptr->emplace<std::string>(3, 'd') == "ddd");

        any_ptr->emplace<std::string>(3, 'd') = "def";
        REQUIRE(any_ptr->get<std::string>() == "def");
    }

    SECTION("move") {
        constexpr int test_data = 37;
        stl_ext::shared_any any = test_data;
        REQUIRE(any.get<int>() == test_data);
        REQUIRE(any.use_count() == 1);

        stl_ext::shared_any copy(std::move(any));
        REQUIRE(any.has_value() == false);  // NOLINT
        REQUIRE(copy.has_value() == true);
        REQUIRE(copy.get<int>() == test_data);
        REQUIRE(any.use_count() == 0);
        REQUIRE(copy.use_count() == 1);

        copy.reset();
        any = stl_ext::shared_any(test_data);
        REQUIRE(any.has_value() == true);
        REQUIRE(any.get<int>() == test_data);
        REQUIRE(copy.has_value() == false);
        REQUIRE(any.use_count() == 1);
        REQUIRE(copy.use_count() == 0);

        copy = std::move(any);
        REQUIRE(any.has_value() == false);  // NOLINT
        REQUIRE(copy.has_value() == true);
        REQUIRE(copy.get<int>() == test_data);
        REQUIRE(any.use_count() == 0);
        REQUIRE(copy.use_count() == 1);

        const auto test_data2 = std::string("abc");
        any = stl_ext::shared_any(test_data2);
        REQUIRE(any.has_value() == true);
        REQUIRE(any.get<std::string>() == test_data2);

        copy = std::move(any);
        REQUIRE(any.has_value() == true);      // NOLINT
        REQUIRE(any.get<int>() == test_data);  // NOLINT
        REQUIRE(copy.has_value() == true);
        REQUIRE(copy.get<std::string>() == test_data2);
        REQUIRE(any.use_count() == 1);
        REQUIRE(copy.use_count() == 1);
    }

    SECTION("assign objects") {
        stl_ext::shared_any any;
        const auto& const_any = any;

        constexpr int test_data1 = 37;
        any = test_data1;
        REQUIRE(any.get<int>() == test_data1);
        REQUIRE(const_any.get<int>() == test_data1);

        const auto test_data2 = std::string("abc");
        any = test_data2;
        REQUIRE(any.get<std::string>() == test_data2);
        REQUIRE(const_any.get<std::string>() == test_data2);

        constexpr char test_data3 = 'N';
        any = test_data3;
        REQUIRE(any.get<char>() == test_data3);
        REQUIRE(const_any.get<char>() == test_data3);
    }

    SECTION("swap two objects") {
        constexpr int test_data1 = 37;
        const auto test_data2 = std::string("abc");
        stl_ext::shared_any any1 = test_data1;
        stl_ext::shared_any any2 = test_data2;
        std::swap(any1, any2);
        REQUIRE(any1.get<std::string>() == test_data2);
        REQUIRE(any2.get<int>() == test_data1);
    }
}
