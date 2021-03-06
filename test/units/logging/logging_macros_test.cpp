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
 * \brief test of logging macros
 */
#include "mprpc/logging/logging_macros.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "mprpc/logging/labeled_logger.h"

namespace {

void test_mprpc_log(mprpc::logging::logger* logger_ptr) {
    constexpr int value = 37;
#line 5 "test.cpp"
    MPRPC_LOG(logger_ptr, mprpc::logging::log_level::info, "value: {}", value);
}

class test_logger final : public mprpc::logging::logger {
public:
    explicit test_logger(mprpc::logging::log_level log_output_level)
        : mprpc::logging::logger(log_output_level) {}

    void write_impl(const char* label, const char* filename, std::uint32_t line,
        const char* function, mprpc::logging::log_level level,
        const char* message) noexcept override {
        this->label = label;
        this->filename = filename;
        this->line = line;
        this->function = function;
        this->level = level;
        this->message = message;
    }

    void write_impl(const char* filename, std::uint32_t line,
        const char* function, mprpc::logging::log_level level,
        const char* message) noexcept override {
        this->filename = filename;
        this->line = line;
        this->function = function;
        this->level = level;
        this->message = message;
    }

    void write_impl(mprpc::logging::log_level level,
        const char* message) noexcept override {
        this->level = level;
        this->message = message;
    }

    std::string label{};
    std::string filename{};
    std::uint32_t line{0};
    std::string function{};
    mprpc::logging::log_level level{mprpc::logging::log_level::trace};
    std::string message{};
};

}  // namespace

TEST_CASE("logging macros in mpRPC") {
    SECTION("macro to write log with logger class") {
        using mprpc::logging::log_level;
        test_logger logger(log_level::trace);

        test_mprpc_log(&logger);
        REQUIRE(logger.filename == "test.cpp");
        REQUIRE(logger.line == 5);
        REQUIRE_THAT(
            logger.function, Catch::Matchers::Contains("test_mprpc_log"));
        REQUIRE(logger.level == log_level::info);
        REQUIRE(logger.message == "value: 37");

        MPRPC_TRACE(&logger, "test");
        REQUIRE(logger.level == log_level::trace);
        REQUIRE(logger.message == "test");

        MPRPC_DEBUG(&logger, "test");
        REQUIRE(logger.level == log_level::debug);
        REQUIRE(logger.message == "test");

        MPRPC_INFO(&logger, "test");
        REQUIRE(logger.level == log_level::info);
        REQUIRE(logger.message == "test");

        MPRPC_WARN(&logger, "test");
        REQUIRE(logger.level == log_level::warn);
        REQUIRE(logger.message == "test");

        MPRPC_ERROR(&logger, "test");
        REQUIRE(logger.level == log_level::error);
        REQUIRE(logger.message == "test");

        MPRPC_FATAL(&logger, "test");
        REQUIRE(logger.level == log_level::fatal);
        REQUIRE(logger.message == "test");
    }

    SECTION("macro to write log with labeled_logger class") {
        using mprpc::logging::log_level;
        test_logger logger(log_level::trace);
        const auto base_logger =
            std::make_shared<test_logger>(log_level::trace);
        mprpc::logging::labeled_logger labeled_logger{
            base_logger, "mprpc_test"};

        MPRPC_INFO(labeled_logger, "test");
        REQUIRE(base_logger->label == "mprpc_test");
    }
}
