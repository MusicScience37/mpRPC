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
 * \brief test of response_future class
 */
#include "mprpc/response_future.h"

#include <future>

#include <catch2/catch.hpp>

#include "create_logger.h"
#include "mprpc/logging/logging_macros.h"

TEST_CASE("mprpc::impl::response_future_data") {
    const auto logger = create_logger("mprpc::impl::response_future_data");

    const auto threads = std::make_shared<mprpc::thread_pool>(logger, 1);
    threads->start();

    std::promise<mprpc::message_data> promise;
    auto future = promise.get_future();
    const auto handler = [&promise](const mprpc::error_info& error,
                             const mprpc::message_data& msg) {
        if (error) {
            promise.set_exception(
                std::make_exception_ptr(mprpc::exception(error)));
        } else {
            promise.set_value(msg);
        }
    };

    static const auto timeout = std::chrono::seconds(3);

    mprpc::impl::response_future_data future_data{*threads};

    SECTION("set error first, and set handler") {
        REQUIRE_NOTHROW(future_data.set_error(mprpc::error_info(
            mprpc::error_code::unexpected_error, "test error")));
        REQUIRE_NOTHROW(future_data.set_handler(handler));

        REQUIRE(future.wait_for(timeout) == std::future_status::ready);
        REQUIRE_THROWS_WITH(future.get(), Catch::Contains("test error"));
    }

    SECTION("set data first, and set handler") {
        const auto response_data = std::string({char(0xC2)});  // false
        const auto response =
            mprpc::message_data(response_data.data(), response_data.size());

        REQUIRE_NOTHROW(future_data.set_response(response));
        REQUIRE_NOTHROW(future_data.set_handler(handler));

        REQUIRE(future.wait_for(timeout) == std::future_status::ready);
        REQUIRE(future.get() == response);
    }

    SECTION("set handler first, and set error") {
        REQUIRE_NOTHROW(future_data.set_handler(handler));
        REQUIRE_NOTHROW(future_data.set_error(mprpc::error_info(
            mprpc::error_code::unexpected_error, "test error")));

        REQUIRE(future.wait_for(timeout) == std::future_status::ready);
        REQUIRE_THROWS_WITH(future.get(), Catch::Contains("test error"));
    }

    SECTION("set handler first, and set data") {
        const auto response_data = std::string({char(0xC2)});  // false
        const auto response =
            mprpc::message_data(response_data.data(), response_data.size());

        REQUIRE_NOTHROW(future_data.set_handler(handler));
        REQUIRE_NOTHROW(future_data.set_response(response));

        REQUIRE(future.wait_for(timeout) == std::future_status::ready);
        REQUIRE(future.get() == response);
    }

    SECTION("duplicate error and response") {
        const auto response_data = std::string({char(0xC2)});  // false
        const auto response =
            mprpc::message_data(response_data.data(), response_data.size());

        REQUIRE_NOTHROW(future_data.set_error(mprpc::error_info(
            mprpc::error_code::unexpected_error, "test error")));
        REQUIRE_THROWS(future_data.set_response(response));
    }

    SECTION("duplicate handlers") {
        REQUIRE_NOTHROW(future_data.set_handler(handler));
        REQUIRE_THROWS(future_data.set_handler(handler));
    }

    threads->stop();
}

TEST_CASE("mprpc::response_future") {
    const auto logger = create_logger("mprpc::response_future");

    const auto threads = std::make_shared<mprpc::thread_pool>(logger, 1);
    threads->start();

    const auto data =
        std::make_shared<mprpc::impl::response_future_data>(*threads);

    static const auto timeout = std::chrono::seconds(3);

    // sample response
    const auto response_data = std::string({char(0xC2)});  // false
    const auto response =
        mprpc::message_data(response_data.data(), response_data.size());

    SECTION("use of empty future") {
        mprpc::response_future response_future;
        REQUIRE_THROWS(response_future.then(
            [](const mprpc::error_info&, const mprpc::message_data&) {}));
        REQUIRE_THROWS(response_future.then([](const mprpc::message_data&) {},
            [](const mprpc::error_info&) {}));
    }

    SECTION("use of a handler") {
        mprpc::response_future response_future{data};

        std::promise<mprpc::message_data> promise;
        auto future = promise.get_future();
        REQUIRE_NOTHROW(
            response_future.then([&promise](const mprpc::error_info& error,
                                     const mprpc::message_data& msg) {
                if (error) {
                    promise.set_exception(
                        std::make_exception_ptr(mprpc::exception(error)));
                } else {
                    promise.set_value(msg);
                }
            }));

        SECTION("on failure") {
            REQUIRE_NOTHROW(data->set_error(mprpc::error_info(
                mprpc::error_code::unexpected_error, "test error")));

            REQUIRE(future.wait_for(timeout) == std::future_status::ready);
            REQUIRE_THROWS_WITH(future.get(), Catch::Contains("test error"));
        }

        SECTION("on success") {
            REQUIRE_NOTHROW(data->set_response(response));

            REQUIRE(future.wait_for(timeout) == std::future_status::ready);
            REQUIRE(future.get() == response);
        }
    }

    SECTION("use of separate handlers") {
        mprpc::response_future response_future{data};

        std::promise<mprpc::message_data> promise;
        auto future = promise.get_future();
        REQUIRE_NOTHROW(response_future.then(
            [&promise](
                const mprpc::message_data& msg) { promise.set_value(msg); },
            [&promise](const mprpc::error_info& error) {
                promise.set_exception(
                    std::make_exception_ptr(mprpc::exception(error)));
            }));

        SECTION("on failure") {
            REQUIRE_NOTHROW(data->set_error(mprpc::error_info(
                mprpc::error_code::unexpected_error, "test error")));

            REQUIRE(future.wait_for(timeout) == std::future_status::ready);
            REQUIRE_THROWS_WITH(future.get(), Catch::Contains("test error"));
        }

        SECTION("on success") {
            REQUIRE_NOTHROW(data->set_response(response));

            REQUIRE(future.wait_for(timeout) == std::future_status::ready);
            REQUIRE(future.get() == response);
        }
    }

    SECTION("use of std::future") {
        mprpc::response_future response_future{data};

        auto future = response_future.get_future();

        SECTION("on failure") {
            REQUIRE_NOTHROW(data->set_error(mprpc::error_info(
                mprpc::error_code::unexpected_error, "test error")));

            REQUIRE(future.wait_for(timeout) == std::future_status::ready);
            REQUIRE_THROWS_WITH(future.get(), Catch::Contains("test error"));
        }

        SECTION("on success") {
            REQUIRE_NOTHROW(data->set_response(response));

            REQUIRE(future.wait_for(timeout) == std::future_status::ready);
            REQUIRE(future.get() == response);
        }
    }

    threads->stop();
}

TEST_CASE("mprpc::response_promise") {
    const auto logger = create_logger("mprpc::response_promise");

    const auto threads = std::make_shared<mprpc::thread_pool>(logger, 1);
    threads->start();

    static const auto timeout = std::chrono::seconds(3);

    // sample response
    const auto response_data = std::string({char(0xC2)});  // false
    const auto response =
        mprpc::message_data(response_data.data(), response_data.size());

    SECTION("use promise") {
        mprpc::response_promise promise(*threads);
        std::future<mprpc::message_data> future;
        REQUIRE_NOTHROW(future = promise.get_future().get_future());

        SECTION("set error") {
            REQUIRE_NOTHROW(promise.set_error(mprpc::error_info(
                mprpc::error_code::unexpected_error, "test error")));

            REQUIRE(future.wait_for(timeout) == std::future_status::ready);
            REQUIRE_THROWS_WITH(future.get(), Catch::Contains("test error"));
        }

        SECTION("set response") {
            REQUIRE_NOTHROW(promise.set_response(response));

            REQUIRE(future.wait_for(timeout) == std::future_status::ready);
            REQUIRE(future.get() == response);
        }
    }

    threads->stop();
}