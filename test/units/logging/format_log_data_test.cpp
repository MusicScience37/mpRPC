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
 * \brief test of format_log_data function
 */
#include "mprpc/logging/format_log_data.h"

#include <catch2/catch_test_macros.hpp>

namespace {

class string_wrapper {
public:
    explicit string_wrapper(std::string data) : data_(std::move(data)) {}

    const std::string& data() const { return data_; }

private:
    std::string data_;
};

}  // namespace

namespace mprpc {
namespace logging {

template <>
struct log_data_traits<string_wrapper> {
    static const std::string& preprocess(const string_wrapper& data) noexcept {
        return data.data();
    }
};

}  // namespace logging
}  // namespace mprpc

TEST_CASE("mprpc::logging::format_log_data") {
    SECTION("one argument with std::string") {
        const auto test_str = std::string("abc");
        REQUIRE(mprpc::logging::format_log_data(test_str) == test_str);
    }

    SECTION("one argument with raw string") {
        REQUIRE(mprpc::logging::format_log_data("abc") == "abc");
    }

    SECTION("format int") {
        constexpr int val = 37;
        REQUIRE(mprpc::logging::format_log_data("{}s", val) == "37s");
    }

    SECTION("format int and double") {
        constexpr int val1 = 37;
        constexpr double val2 = 1.0;
        REQUIRE(mprpc::logging::format_log_data("{} {:.1}", val1, val2) ==
            "37 1.0");
    }

    SECTION("format user defined type") {
        REQUIRE(mprpc::logging::format_log_data(
                    "data: {}", string_wrapper("abc")) == "data: abc");
    }
}
