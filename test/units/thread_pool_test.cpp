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
 * \brief test of thread_pool class
 */
#include "mprpc/thread_pool.h"

#include <atomic>
#include <memory>
#include <stdexcept>
#include <thread>

#include <catch2/catch_test_macros.hpp>

#include "create_logger.h"
#include "mprpc/exception.h"
#include "mprpc/logging/logging_macros.h"

TEST_CASE("mprpc::thread_pool") {
    const auto logger = create_logger("mprpc::thread_pool");

    std::atomic<int> error_count{0};
    auto on_error = [&error_count](
                        const mprpc::error_info& /*unused*/) { ++error_count; };

    SECTION("start and stop threads") {
        auto threads = std::make_shared<mprpc::thread_pool>(logger, 2);
        threads->on_error(on_error);
        threads->start();
        threads->stop();
        threads.reset();
        REQUIRE(error_count.load() == 0);
    }

    SECTION("start and stop threads in destructors") {
        auto threads = std::make_shared<mprpc::thread_pool>(logger, 2);
        threads->on_error(on_error);
        threads->start();
        threads.reset();
        REQUIRE(error_count.load() == 0);
    }

    SECTION("errors in thread") {
        auto threads = std::make_shared<mprpc::thread_pool>(logger, 2);
        threads->on_error(on_error);
        threads->start();

        threads->post([&logger] {
            MPRPC_INFO(logger, "throws an exception with mprpc::exception");
            throw mprpc::exception(
                mprpc::error_info(mprpc::error_code::parse_error,
                    "test of exception in threads"));
        });

        threads->post([&logger] {
            MPRPC_INFO(logger, "throws an exception with std::runtime_error");
            throw std::runtime_error("test of exception in threads");
        });

        // wait for starting execution
        const auto duration = std::chrono::milliseconds(100);
        std::this_thread::sleep_for(duration);
        threads.reset();
        REQUIRE(error_count.load() == 2);
    }
}
