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
 * \brief test of macros to get positions in source codes
 */
#include "mprpc/position_macros.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace {

// NOLINTNEXTLINE
std::string test_func_name() { return std::string(MPRPC_FUNCTION); }

struct macro_test_class {
    macro_test_class() : result(MPRPC_FUNCTION) {}
    std::string result;
};

}  // namespace

TEST_CASE("position macros") {
    SECTION("macros to get source information") {
#line 3 "test.cpp"
        REQUIRE(std::string(MPRPC_FILENAME) == "test.cpp");
        REQUIRE(MPRPC_LINE == 4);
        REQUIRE_THAT(
            test_func_name(), Catch::Matchers::Contains("test_func_name"));
    }

    SECTION("function name of constructor") {
        macro_test_class obj;
        REQUIRE_THAT(obj.result, Catch::Matchers::Contains("macro_test_class"));
    }
}
