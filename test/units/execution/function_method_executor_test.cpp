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
 * \brief test of function_method_executor class
 */
#include "mprpc/execution/function_method_executor.h"

#include <catch2/catch_test_macros.hpp>

#include "../create_logger.h"

TEST_CASE("mprpc::execution::function_method_executor") {
    const auto logger =
        create_logger("mprpc::execution::function_method_executor");

    SECTION("request to function with one parameter and result") {
        const auto name = std::string("abc");
        const auto func = [](int x) { return x; };
        const auto method =
            mprpc::execution::make_function_method_executor<int(int)>(
                logger, name, func);
        REQUIRE(method->name() == name);

        constexpr int param1 = 12345;
        std::shared_ptr<mprpc::transport::session> session;  // not used

        constexpr mprpc::msgid_t msgid = 37;
        const auto request = mprpc::message(
            mprpc::pack_request(msgid, name, std::make_tuple(param1)));
        mprpc::message_data response_data;
        REQUIRE_NOTHROW(
            response_data = method->process_request(session, request));

        mprpc::message response_message;
        REQUIRE_NOTHROW(response_message = mprpc::message(response_data));
        REQUIRE(response_message.type() == mprpc::msgtype::response);
        REQUIRE(response_message.msgid() == msgid);
        REQUIRE(response_message.has_error() == false);
        REQUIRE(response_message.result_as<int>() == param1);
    }

    SECTION("request to function with two parameters and result") {
        const auto name = std::string("abc");
        const auto func = [](int x, int y) { return x + y; };
        const auto method =
            mprpc::execution::make_function_method_executor<int(int, int)>(
                logger, name, func);
        REQUIRE(method->name() == name);

        constexpr int param1 = 12345;
        constexpr int param2 = -54321;
        std::shared_ptr<mprpc::transport::session> session;  // not used

        constexpr mprpc::msgid_t msgid = 37;
        const auto request = mprpc::message(
            mprpc::pack_request(msgid, name, std::make_tuple(param1, param2)));
        mprpc::message_data response_data;
        REQUIRE_NOTHROW(
            response_data = method->process_request(session, request));

        mprpc::message response_message;
        REQUIRE_NOTHROW(response_message = mprpc::message(response_data));
        REQUIRE(response_message.type() == mprpc::msgtype::response);
        REQUIRE(response_message.msgid() == msgid);
        REQUIRE(response_message.has_error() == false);
        REQUIRE(response_message.result_as<int>() == param1 + param2);
    }

    SECTION(
        "request to function with one parameter and result, but throws "
        "exception") {
        const auto name = std::string("abc");
        const auto func = [](int x) {
            if (x == 0) {
                throw std::runtime_error("test");
            }
            return x;
        };
        const auto method =
            mprpc::execution::make_function_method_executor<int(int)>(
                logger, name, func);
        REQUIRE(method->name() == name);

        constexpr int param1 = 0;
        std::shared_ptr<mprpc::transport::session> session;  // not used

        constexpr mprpc::msgid_t msgid = 37;
        const auto request = mprpc::message(
            mprpc::pack_request(msgid, name, std::make_tuple(param1)));
        mprpc::message_data response_data;
        REQUIRE_NOTHROW(
            response_data = method->process_request(session, request));

        mprpc::message response_message;
        REQUIRE_NOTHROW(response_message = mprpc::message(response_data));
        REQUIRE(response_message.type() == mprpc::msgtype::response);
        REQUIRE(response_message.msgid() == msgid);
        REQUIRE(response_message.has_error());
        REQUIRE(response_message.error_as<std::string>() == "test");
    }

    SECTION("notification to function with one parameter and result") {
        const auto name = std::string("abc");
        int received_param = 0;
        const auto func = [&received_param](int x) {
            received_param = x;
            return x;
        };
        const auto method =
            mprpc::execution::make_function_method_executor<int(int)>(
                logger, name, func);
        REQUIRE(method->name() == name);

        constexpr int param1 = 12345;
        std::shared_ptr<mprpc::transport::session> session;  // not used
        const auto notification = mprpc::message(
            mprpc::pack_notification(name, std::make_tuple(param1)));
        REQUIRE_NOTHROW(method->process_notification(session, notification));
        REQUIRE(received_param == param1);
    }

    SECTION(
        "notification to function with one parameter and result, but throws "
        "exception") {
        const auto name = std::string("abc");
        int received_param = 0;
        const auto func = [&received_param](int x) {
            received_param = x;
            if (x == 0) {
                throw std::runtime_error("test");
            }
            return x;
        };
        const auto method =
            mprpc::execution::make_function_method_executor<int(int)>(
                logger, name, func);
        REQUIRE(method->name() == name);

        constexpr int param1 = 0;
        std::shared_ptr<mprpc::transport::session> session;  // not used
        const auto notification = mprpc::message(
            mprpc::pack_notification(name, std::make_tuple(param1)));
        REQUIRE_NOTHROW(method->process_notification(session, notification));
        REQUIRE(received_param == param1);
    }

    SECTION("request to function with one parameter and no result") {
        const auto name = std::string("abc");
        std::string received_param;
        const auto func = [&received_param](std::string str) {
            received_param = std::move(str);
        };
        const auto method =
            mprpc::execution::make_function_method_executor<void(std::string)>(
                logger, name, func);
        REQUIRE(method->name() == name);

        const auto param1 = std::string("def");
        std::shared_ptr<mprpc::transport::session> session;  // not used

        constexpr mprpc::msgid_t msgid = 37;
        const auto request = mprpc::message(
            mprpc::pack_request(msgid, name, std::make_tuple(param1)));
        mprpc::message_data response_data;
        REQUIRE_NOTHROW(
            response_data = method->process_request(session, request));
        REQUIRE(received_param == param1);

        mprpc::message response_message;
        REQUIRE_NOTHROW(response_message = mprpc::message(response_data));
        REQUIRE(response_message.type() == mprpc::msgtype::response);
        REQUIRE(response_message.msgid() == msgid);
        REQUIRE(response_message.has_error() == false);
        REQUIRE(response_message.result().is_nil());
    }

    SECTION("request to function with two parameters and no result") {
        const auto name = std::string("abc");
        std::tuple<std::string, int> received_param;
        const auto func = [&received_param](std::string str, int x) {
            received_param = std::make_tuple(std::move(str), x);
        };
        const auto method =
            mprpc::execution::make_function_method_executor<void(
                std::string, int)>(logger, name, func);
        REQUIRE(method->name() == name);

        const auto param = std::make_tuple(std::string("def"), 7);
        std::shared_ptr<mprpc::transport::session> session;  // not used

        constexpr mprpc::msgid_t msgid = 37;
        const auto request =
            mprpc::message(mprpc::pack_request(msgid, name, param));
        mprpc::message_data response_data;
        REQUIRE_NOTHROW(
            response_data = method->process_request(session, request));
        REQUIRE(received_param == param);

        mprpc::message response_message;
        REQUIRE_NOTHROW(response_message = mprpc::message(response_data));
        REQUIRE(response_message.type() == mprpc::msgtype::response);
        REQUIRE(response_message.msgid() == msgid);
        REQUIRE(response_message.has_error() == false);
        REQUIRE(response_message.result().is_nil());
    }

    SECTION(
        "request to function with one parameter and no result, but throws "
        "exception") {
        const auto name = std::string("abc");
        const auto func = [](const std::string& str) {
            throw std::runtime_error(str);
        };
        const auto method =
            mprpc::execution::make_function_method_executor<void(std::string)>(
                logger, name, func);
        REQUIRE(method->name() == name);

        const auto param1 = std::string("def");
        std::shared_ptr<mprpc::transport::session> session;  // not used

        constexpr mprpc::msgid_t msgid = 37;
        const auto request = mprpc::message(
            mprpc::pack_request(msgid, name, std::make_tuple(param1)));
        mprpc::message_data response_data;
        REQUIRE_NOTHROW(
            response_data = method->process_request(session, request));

        mprpc::message response_message;
        REQUIRE_NOTHROW(response_message = mprpc::message(response_data));
        REQUIRE(response_message.type() == mprpc::msgtype::response);
        REQUIRE(response_message.msgid() == msgid);
        REQUIRE(response_message.has_error());
        REQUIRE(response_message.error_as<std::string>() == param1);
    }

    SECTION("notification to function with one parameter and no result") {
        const auto name = std::string("abc");
        std::string received_param;
        const auto func = [&received_param](std::string str) {
            received_param = std::move(str);
        };
        const auto method =
            mprpc::execution::make_function_method_executor<void(std::string)>(
                logger, name, func);
        REQUIRE(method->name() == name);

        const auto param1 = std::string("def");
        std::shared_ptr<mprpc::transport::session> session;  // not used

        const auto request = mprpc::message(
            mprpc::pack_notification(name, std::make_tuple(param1)));
        REQUIRE_NOTHROW(method->process_notification(session, request));
        REQUIRE(received_param == param1);
    }

    SECTION(
        "notification to function with one parameter and no result, but throws "
        "exception") {
        const auto name = std::string("abc");
        const auto func = [](const std::string& str) {
            throw std::runtime_error(str);
        };
        const auto method =
            mprpc::execution::make_function_method_executor<void(std::string)>(
                logger, name, func);
        REQUIRE(method->name() == name);

        const auto param1 = std::string("def");
        std::shared_ptr<mprpc::transport::session> session;  // not used

        const auto request = mprpc::message(
            mprpc::pack_notification(name, std::make_tuple(param1)));
        REQUIRE_NOTHROW(method->process_notification(session, request));
    }
}
