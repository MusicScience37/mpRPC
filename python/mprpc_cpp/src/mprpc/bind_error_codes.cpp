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
    using mprpc::error_code::common_errors;
    pybind11::enum_<common_errors>(module, "CommonErrors",
        "enumeration of common error codes", pybind11::arithmetic())
        .value("SUCCESS", common_errors::success, "success (no error)")
        .value("UNEXPECTED_ERROR", common_errors::unexpected_error,
            "unexpected error")
        .value("UNEXPECTED_NULLPTR", common_errors::unexpected_nullptr,
            "unexpected null pointer")
        .export_values();

    using mprpc::error_code::message_errors;
    pybind11::enum_<message_errors>(module, "MessageErrors",
        "enumeration of message-related error codes", pybind11::arithmetic())
        .value("PARSE_ERROR", message_errors::parse_error, "parse error")
        .value("INVALID_MESSAGE", message_errors::invalid_message,
            "invalid message")
        .export_values();

    using mprpc::error_code::network_errors;
    pybind11::enum_<network_errors>(module, "NetworkErrors",
        "enumeration of network-related error codes", pybind11::arithmetic())
        .value("EOF", network_errors::eof, "end of file")
        .value("FAILED_TO_LISTEN", network_errors::failed_to_listen,
            "failed to listen to an address")
        .value("FAILED_TO_ACCEPT", network_errors::failed_to_accept,
            "failed to accept a connection")
        .value("FAILED_TO_RESOLVE", network_errors::failed_to_resolve,
            "failed to resolve a server")
        .value("FAILED_TO_CONNECT", network_errors::failed_to_connect,
            "failed to connect to a server")
        .value("FAILED_TO_READ", network_errors::failed_to_read,
            "failed to read data")
        .value("FAILED_TO_WRITE", network_errors::failed_to_write,
            "failed to write data")
        .export_values();

    using mprpc::error_code::execution_errors;
    pybind11::enum_<execution_errors>(module, "ExecutionErrors",
        "enumeration of execution-related error codes", pybind11::arithmetic())
        .value("METHOD_NOT_FOUND", execution_errors::method_not_found,
            "method not found")
        .export_values();

    using mprpc::error_code::future_errors;
    pybind11::enum_<future_errors>(module, "FutureErrors",
        "enumeration of future-related error codes", pybind11::arithmetic())
        .value("INVALID_FUTURE_USE", future_errors::invalid_future_use,
            "use of invalid future")
        .export_values();

    using mprpc::error_code::config_errors;
    pybind11::enum_<config_errors>(module, "ConfigErrors",
        "enumeration of configuration-related error codes",
        pybind11::arithmetic())
        .value("INVALID_CONFIG_VALUE", config_errors::invalid_config_value,
            "invalid configuration value")
        .value("CONFIG_PARSE_ERROR", config_errors::config_parse_error,
            "parse error of configuration files")
        .export_values();

    using mprpc::error_code::client_errors;
    pybind11::enum_<client_errors>(module, "ClientErrors",
        "enumeration of client-specific error codes", pybind11::arithmetic())
        .value("TIMEOUT", client_errors::timeout, "timeout")
        .export_values();
}

}  // namespace python
}  // namespace mprpc
