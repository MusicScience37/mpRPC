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
 * \brief test of logger class
 */
#include "mprpc/logging/logger.h"

#include <catch2/catch_test_macros.hpp>

namespace {

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

TEST_CASE("mprpc::logging::logger") {
    SECTION("is_outputted_level") {
        using mprpc::logging::log_level;
        test_logger logger(log_level::info);
        REQUIRE(logger.log_output_level() == log_level::info);
        REQUIRE(logger.is_outputted_level(log_level::debug) == false);
        REQUIRE(logger.is_outputted_level(log_level::info) == true);
        REQUIRE(logger.is_outputted_level(log_level::warn) == true);
    }

    SECTION("write without source information") {
        using mprpc::logging::log_level;
        test_logger logger(log_level::info);

        logger.write(log_level::debug, "test");
        REQUIRE(logger.message == "");  // NOLINT

        logger.write(log_level::info, "test");
        REQUIRE(logger.level == log_level::info);
        REQUIRE(logger.message == "test");

        constexpr int value = 37;
        logger.write(log_level::warn, "value: {}", value);
        REQUIRE(logger.level == log_level::warn);
        REQUIRE(logger.message == "value: 37");
    }

    SECTION("write with source information") {
        using mprpc::logging::log_level;
        test_logger logger(log_level::info);

        const char* filename = "test.cpp";
        constexpr std::uint32_t line = 123;
        const char* function = "test_func";

        logger.write(filename, line, function, log_level::debug, "test");
        REQUIRE(logger.message == "");  // NOLINT

        logger.write(filename, line, function, log_level::info, "test");
        REQUIRE(logger.filename == filename);
        REQUIRE(logger.line == line);
        REQUIRE(logger.function == function);
        REQUIRE(logger.level == log_level::info);
        REQUIRE(logger.message == "test");

        constexpr int value = 37;
        logger.write(
            filename, line, function, log_level::warn, "value: {}", value);
        REQUIRE(logger.filename == filename);
        REQUIRE(logger.line == line);
        REQUIRE(logger.function == function);
        REQUIRE(logger.level == log_level::warn);
        REQUIRE(logger.message == "value: 37");
    }
}
