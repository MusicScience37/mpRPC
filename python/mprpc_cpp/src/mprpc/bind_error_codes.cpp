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
 * \brief implementation of bind_error_codes function
 */
#include "mprpc/bind_error_codes.h"

#include <vector>

#include "mprpc/error_code.h"

namespace {

struct unused {};

}  // namespace

namespace mprpc {
namespace python {

void bind_error_codes(pybind11::module& module) {
    static const auto code_list =
        std::vector<std::tuple<mprpc::error_code_t, std::string, std::string>>{
            {mprpc::error_code::success, "SUCCESS", "success (no error)"},
            {mprpc::error_code::unexpected_error, "UNEXPECTED_ERROR",
                "unexpected error"},
            {mprpc::error_code::unexpected_nullptr, "UNEXPECTED_NULLPTR",
                "unexpected null pointer"},
            {mprpc::error_code::parse_error, "PARSE_ERROR", "parse error"},
            {mprpc::error_code::invalid_message, "INVALID_MESSAGE",
                "invalid message"},
            {mprpc::error_code::eof, "EOF", "end of file"},
            {mprpc::error_code::failed_to_listen, "FAILED_TO_LISTEN",
                "failed to listen to an address"},
            {mprpc::error_code::failed_to_accept, "FAILED_TO_ACCEPT",
                "failed to accept a connection"},
            {mprpc::error_code::failed_to_resolve, "FAILED_TO_RESOLVE",
                "failed to resolve a server"},
            {mprpc::error_code::failed_to_connect, "FAILED_TO_CONNECT",
                "failed to connect to a server"},
            {mprpc::error_code::failed_to_read, "FAILED_TO_READ",
                "failed to read data"},
            {mprpc::error_code::failed_to_write, "FAILED_TO_WRITE",
                "failed to write data"},
            {mprpc::error_code::method_not_found, "METHOD_NOT_FOUND",
                "method not found"},
            {mprpc::error_code::invalid_future_use, "INVALID_FUTURE_USE",
                "use of invalid future"},
            {mprpc::error_code::invalid_config_value, "INVALID_CONFIG_VALUE",
                "invalid configuration value"},
            {mprpc::error_code::config_parse_error, "CONFIG_PARSE_ERROR",
                "parse error of configuration files"},
            {mprpc::error_code::timeout, "TIMEOUT", "timeout"}};

    pybind11::class_<unused> codes{
        module, "ErrorCode", "definition of error codes"};
    for (const auto& tuple : code_list) {
        codes.def_readonly_static(std::get<1>(tuple).c_str(),
            &std::get<0>(tuple), std::get<2>(tuple).c_str());
    }
}

}  // namespace python
}  // namespace mprpc
