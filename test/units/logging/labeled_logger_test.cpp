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
 * \brief test of labeled_logger class
 */
#include "mprpc/logging/labeled_logger.h"

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

TEST_CASE("mprpc::logging::labeled_logger") {
    SECTION("check label") {
        const auto logger =
            std::make_shared<test_logger>(mprpc::logging::log_level::info);
        auto labeled_logger = mprpc::logging::labeled_logger(logger);

        logger->label = "a";
        labeled_logger.write(
            "test.cpp", 1, "test", mprpc::logging::log_level::info, "contents");
        REQUIRE(logger->label == "a");
        REQUIRE(logger->filename == "test.cpp");
        REQUIRE(logger->line == 1);
        REQUIRE(logger->function == "test");
        REQUIRE(logger->level == mprpc::logging::log_level::info);
        REQUIRE(logger->message == "contents");

        labeled_logger =
            mprpc::logging::labeled_logger(labeled_logger, "mprpc");
        labeled_logger.write(
            "test.cpp", 1, "test", mprpc::logging::log_level::info, "contents");
        REQUIRE(logger->label == "mprpc");
        REQUIRE(logger->filename == "test.cpp");
        REQUIRE(logger->line == 1);
        REQUIRE(logger->function == "test");
        REQUIRE(logger->level == mprpc::logging::log_level::info);
        REQUIRE(logger->message == "contents");

        labeled_logger = mprpc::logging::labeled_logger(labeled_logger, "test");
        labeled_logger.write(
            "test.cpp", 1, "test", mprpc::logging::log_level::info, "contents");
        REQUIRE(logger->label == "mprpc.test");
        REQUIRE(logger->filename == "test.cpp");
        REQUIRE(logger->line == 1);
        REQUIRE(logger->function == "test");
        REQUIRE(logger->level == mprpc::logging::log_level::info);
        REQUIRE(logger->message == "contents");
    }
}
