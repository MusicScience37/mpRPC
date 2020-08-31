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
 * \brief test of spdlog_logger class
 */
#include "mprpc/logging/spdlog_logger.h"

#include <algorithm>
#include <regex>
#include <sstream>

#include <catch2/catch.hpp>
#include <spdlog/sinks/ostream_sink.h>

#include "mprpc/logging/logging_macros.h"

namespace {

void test_mprpc_log(const std::shared_ptr<mprpc::logging::logger>& logger_ptr) {
    constexpr int value = 37;
#line 5 "test.cpp"
    MPRPC_INFO(logger_ptr, "value: {}", value);
}

template <typename T>
void discard_forcefully(T&& /*unused*/) {}

std::string remove_line_feed(std::string str) {
    discard_forcefully(std::remove(str.begin(), str.end(), '\r'));
    auto iter = std::remove(str.begin(), str.end(), '\n');
    str.erase(iter, str.end());
    return str;
}

}  // namespace

TEST_CASE("mprpc::logging::spdlog_logger") {
    SECTION("write log") {
        std::ostringstream stream;
        auto spdlog_logger = spdlog::synchronous_factory::template create<
            spdlog::sinks::ostream_sink_mt>("test_spdlog_logger", stream);
        auto logger = std::make_shared<mprpc::logging::spdlog_logger>(
            spdlog_logger, mprpc::logging::log_level::info);

        test_mprpc_log(logger);
        auto log = remove_line_feed(stream.str());
        REQUIRE_THAT(log,
            Catch::Matches(
                R"***(\[\d\d\d\d-\d\d-\d\d \d\d:\d\d:\d\d\.\d\d\d\d\d\d\] )***"
                R"***(\[info\]  \(thread \d*\) value: 37 \(test.cpp:5\))***"));

        stream.str("");
        logger->write(mprpc::logging::log_level::error, "message");
        log = remove_line_feed(stream.str());
        REQUIRE_THAT(log,
            Catch::Matches(
                R"***(\[\d\d\d\d-\d\d-\d\d \d\d:\d\d:\d\d\.\d\d\d\d\d\d\] )***"
                R"***(\[error\] \(thread \d*\) message)***"));
    }

    SECTION("log level string") {
        using mprpc::logging::log_level;
        std::ostringstream stream;
        auto spdlog_logger = spdlog::synchronous_factory::template create<
            spdlog::sinks::ostream_sink_mt>(
            "test_spdlog_logger_log_level", stream);
        auto logger = std::make_shared<mprpc::logging::spdlog_logger>(
            spdlog_logger, log_level::trace);

        stream.str("");
        logger->write(log_level::trace, "message");
        auto log = stream.str();
        log.pop_back();
        REQUIRE_THAT(log, Catch::Contains("] [trace] ("));

        stream.str("");
        logger->write(log_level::debug, "message");
        log = stream.str();
        log.pop_back();
        REQUIRE_THAT(log, Catch::Contains("] [debug] ("));

        stream.str("");
        logger->write(log_level::info, "message");
        log = stream.str();
        log.pop_back();
        REQUIRE_THAT(log, Catch::Contains("] [info]  ("));

        stream.str("");
        logger->write(log_level::warn, "message");
        log = stream.str();
        log.pop_back();
        REQUIRE_THAT(log, Catch::Contains("] [warn]  ("));

        stream.str("");
        logger->write(log_level::error, "message");
        log = stream.str();
        log.pop_back();
        REQUIRE_THAT(log, Catch::Contains("] [error] ("));

        stream.str("");
        logger->write(log_level::fatal, "message");
        log = stream.str();
        log.pop_back();
        REQUIRE_THAT(log, Catch::Contains("] [fatal] ("));
    }

    SECTION("create logger to write to file") {
        std::shared_ptr<mprpc::logging::logger> logger;
        REQUIRE_NOTHROW(
            logger = mprpc::logging::create_file_logger(
                "test_spdlog_logger_file", "test_spdlog_logger.log"));

        logger->write(mprpc::logging::log_level::error, "testing logger");
    }

    SECTION("create logger to write to stdout") {
        REQUIRE_NOTHROW(
            mprpc::logging::create_stdout_logger("test_spdlog_logger_stdout"));
    }
}
