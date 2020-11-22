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
 * \brief test of typed_response_future class
 */
#include "mprpc/typed_response_future.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "create_logger.h"
#include "mprpc/pack_data.h"

TEST_CASE("mprpc::typed_response_future") {
    auto logger = create_logger("mprpc::response_promise");

    const auto threads = std::make_shared<mprpc::thread_pool>(logger, 1);
    threads->start();

    static const auto timeout = std::chrono::seconds(3);

    // sample responses
    const int error_data = 37;
    const auto error_response =
        mprpc::message(mprpc::pack_error_response(0, error_data));
    const auto response =
        mprpc::message(mprpc::pack_response(0, std::string("abc")));

    SECTION("use of a handler") {
        mprpc::response_promise response_promise(*threads);
        mprpc::typed_response_future<std::string> response_future;
        REQUIRE_NOTHROW(
            response_future = mprpc::typed_response_future<std::string>(
                response_promise.get_future()));

        std::promise<std::string> result_promise;
        auto result_future = result_promise.get_future();
        REQUIRE_NOTHROW(response_future.then(
            [&result_promise](
                const mprpc::error_info& error, const std::string& result) {
                if (error) {
                    result_promise.set_exception(
                        std::make_exception_ptr(mprpc::exception(error)));
                } else {
                    result_promise.set_value(result);
                }
            }));

        SECTION("on client error") {
            response_promise.set_error(mprpc::error_info(
                mprpc::error_code::unexpected_error, "test error"));

            REQUIRE(
                result_future.wait_for(timeout) == std::future_status::ready);
            REQUIRE_THROWS_WITH(
                result_future.get(), Catch::Matchers::Contains("test error"));
        }

        SECTION("on server error") {
            response_promise.set_response(error_response);

            REQUIRE(
                result_future.wait_for(timeout) == std::future_status::ready);
            REQUIRE_THROWS_WITH(result_future.get(),
                Catch::Matchers::Contains("error on server"));
        }

        SECTION("on success") {
            response_promise.set_response(response);

            REQUIRE(
                result_future.wait_for(timeout) == std::future_status::ready);
            REQUIRE(result_future.get() == "abc");
        }
    }

    SECTION("use of separate handlers") {
        mprpc::response_promise response_promise(*threads);
        mprpc::typed_response_future<std::string> response_future;
        REQUIRE_NOTHROW(
            response_future = mprpc::typed_response_future<std::string>(
                response_promise.get_future()));

        std::promise<std::string> result_promise;
        auto result_future = result_promise.get_future();
        REQUIRE_NOTHROW(response_future.then(
            [&result_promise](const std::string& result) {
                result_promise.set_value(result);
            },
            [&result_promise](const mprpc::error_info& error) {
                result_promise.set_exception(
                    std::make_exception_ptr(mprpc::exception(error)));
            }));

        SECTION("on client error") {
            response_promise.set_error(mprpc::error_info(
                mprpc::error_code::unexpected_error, "test error"));

            REQUIRE(
                result_future.wait_for(timeout) == std::future_status::ready);
            REQUIRE_THROWS_WITH(
                result_future.get(), Catch::Matchers::Contains("test error"));
        }

        SECTION("on server error") {
            response_promise.set_response(error_response);

            REQUIRE(
                result_future.wait_for(timeout) == std::future_status::ready);
            REQUIRE_THROWS_WITH(result_future.get(),
                Catch::Matchers::Contains("error on server"));
        }

        SECTION("on success") {
            response_promise.set_response(response);

            REQUIRE(
                result_future.wait_for(timeout) == std::future_status::ready);
            REQUIRE(result_future.get() == "abc");
        }
    }

    SECTION("use of std::future") {
        mprpc::response_promise response_promise(*threads);
        std::future<std::string> result_future;
        REQUIRE_NOTHROW(
            result_future = mprpc::typed_response_future<std::string>(
                response_promise.get_future())
                                .get_future());

        SECTION("on client error") {
            response_promise.set_error(mprpc::error_info(
                mprpc::error_code::unexpected_error, "test error"));

            REQUIRE(
                result_future.wait_for(timeout) == std::future_status::ready);
            REQUIRE_THROWS_WITH(
                result_future.get(), Catch::Matchers::Contains("test error"));
        }

        SECTION("on server error") {
            response_promise.set_response(error_response);

            REQUIRE(
                result_future.wait_for(timeout) == std::future_status::ready);
            REQUIRE_THROWS_WITH(result_future.get(),
                Catch::Matchers::Contains("error on server"));
        }

        SECTION("on success") {
            response_promise.set_response(response);

            REQUIRE(
                result_future.wait_for(timeout) == std::future_status::ready);
            REQUIRE(result_future.get() == "abc");
        }
    }
}

TEST_CASE("mprpc::typed_response_future<void>") {
    auto logger = create_logger("mprpc::response_promise<void>");

    const auto threads = std::make_shared<mprpc::thread_pool>(logger, 1);
    threads->start();

    static const auto timeout = std::chrono::seconds(3);

    // sample responses
    const int error_data = 37;
    const auto error_response =
        mprpc::message(mprpc::pack_error_response(0, error_data));
    const auto response = mprpc::message(mprpc::pack_response(0, 0));

    SECTION("use of a handler") {
        mprpc::response_promise response_promise(*threads);
        mprpc::typed_response_future<void> response_future;
        REQUIRE_NOTHROW(response_future = mprpc::typed_response_future<void>(
                            response_promise.get_future()));

        std::promise<void> result_promise;
        auto result_future = result_promise.get_future();
        REQUIRE_NOTHROW(response_future.then(
            [&result_promise](const mprpc::error_info& error) {
                if (error) {
                    result_promise.set_exception(
                        std::make_exception_ptr(mprpc::exception(error)));
                } else {
                    result_promise.set_value();
                }
            }));

        SECTION("on client error") {
            response_promise.set_error(mprpc::error_info(
                mprpc::error_code::unexpected_error, "test error"));

            REQUIRE(
                result_future.wait_for(timeout) == std::future_status::ready);
            REQUIRE_THROWS_WITH(
                result_future.get(), Catch::Matchers::Contains("test error"));
        }

        SECTION("on server error") {
            response_promise.set_response(error_response);

            REQUIRE(
                result_future.wait_for(timeout) == std::future_status::ready);
            REQUIRE_THROWS_WITH(result_future.get(),
                Catch::Matchers::Contains("error on server"));
        }

        SECTION("on success") {
            response_promise.set_response(response);

            REQUIRE(
                result_future.wait_for(timeout) == std::future_status::ready);
            REQUIRE_NOTHROW(result_future.get());
        }
    }

    SECTION("use of separate handlers") {
        mprpc::response_promise response_promise(*threads);
        mprpc::typed_response_future<void> response_future;
        REQUIRE_NOTHROW(response_future = mprpc::typed_response_future<void>(
                            response_promise.get_future()));

        std::promise<void> result_promise;
        auto result_future = result_promise.get_future();
        REQUIRE_NOTHROW(response_future.then(
            [&result_promise]() { result_promise.set_value(); },
            [&result_promise](const mprpc::error_info& error) {
                result_promise.set_exception(
                    std::make_exception_ptr(mprpc::exception(error)));
            }));

        SECTION("on client error") {
            response_promise.set_error(mprpc::error_info(
                mprpc::error_code::unexpected_error, "test error"));

            REQUIRE(
                result_future.wait_for(timeout) == std::future_status::ready);
            REQUIRE_THROWS_WITH(
                result_future.get(), Catch::Matchers::Contains("test error"));
        }

        SECTION("on server error") {
            response_promise.set_response(error_response);

            REQUIRE(
                result_future.wait_for(timeout) == std::future_status::ready);
            REQUIRE_THROWS_WITH(result_future.get(),
                Catch::Matchers::Contains("error on server"));
        }

        SECTION("on success") {
            response_promise.set_response(response);

            REQUIRE(
                result_future.wait_for(timeout) == std::future_status::ready);
            REQUIRE_NOTHROW(result_future.get());
        }
    }

    SECTION("use of std::future") {
        mprpc::response_promise response_promise(*threads);
        std::future<void> result_future;
        REQUIRE_NOTHROW(result_future = mprpc::typed_response_future<void>(
                            response_promise.get_future())
                                            .get_future());

        SECTION("on client error") {
            response_promise.set_error(mprpc::error_info(
                mprpc::error_code::unexpected_error, "test error"));

            REQUIRE(
                result_future.wait_for(timeout) == std::future_status::ready);
            REQUIRE_THROWS_WITH(
                result_future.get(), Catch::Matchers::Contains("test error"));
        }

        SECTION("on server error") {
            response_promise.set_response(error_response);

            REQUIRE(
                result_future.wait_for(timeout) == std::future_status::ready);
            REQUIRE_THROWS_WITH(result_future.get(),
                Catch::Matchers::Contains("error on server"));
        }

        SECTION("on success") {
            response_promise.set_response(response);

            REQUIRE(
                result_future.wait_for(timeout) == std::future_status::ready);
            REQUIRE_NOTHROW(result_future.get());
        }
    }
}
