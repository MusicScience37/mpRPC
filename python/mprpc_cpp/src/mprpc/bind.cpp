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
 * \brief implementation of the wrapper of mpRPC C++ library for Python
 */
#include <memory>

#include <pybind11/pybind11.h>

#include "mprpc/logging/basic_loggers.h"
#include "mprpc/logging/log_level.h"
#include "mprpc/logging/logger.h"
#include "mprpc/python_logger_helper.h"

static void add_logging(pybind11::module& module) {
    using mprpc::logging::log_level;
    pybind11::enum_<log_level>(module, "LogLevel", "enumeration of log levels")
        .value("trace", log_level::trace, "trace")
        .value("debug", log_level::debug, "debug")
        .value("info", log_level::info, "information")
        .value("warn", log_level::warn, "warning")
        .value("error", log_level::error, "error")
        .value("fatal", log_level::fatal, "fatal")
        .value("none", log_level::none, "none (for log output level)")
        .export_values();

    using mprpc::logging::logger;
    // NOLINTNEXTLINE: no member to export
    pybind11::class_<logger, std::shared_ptr<logger>>(
        module, "Logger", "base class of loggers");

    using mprpc::python::python_logger_helper;
    pybind11::class_<python_logger_helper, logger,
        std::shared_ptr<python_logger_helper>>(module, "PythonLoggerHelper",
        "class of loggers for implementation in Python")
        .def(pybind11::init<
                 const std::function<void(const std::string&,
                     const std::string&, std::uint32_t, const std::string&,
                     log_level, const std::string&)>&,
                 log_level>(),
            "construct");

    using mprpc::logging::create_file_logger;
    module.def("create_file_logger", &create_file_logger,
        pybind11::arg("base_filename"),
        pybind11::arg("log_output_level") = log_level::warn,
        pybind11::arg("max_file_size") = mprpc::logging::default_max_file_size,
        pybind11::arg("max_files") = mprpc::logging::default_max_files,
        pybind11::arg("rotate_on_open") = false,
        "create a logger writing to files");

    using mprpc::logging::create_stdout_logger;
    module.def("create_stdout_logger", &create_stdout_logger,
        pybind11::arg("log_output_level") = log_level::warn,
        "create a logger writing to standard output");
}

PYBIND11_MODULE(_mprpc_cpp, module) {
    module.doc() = "wrapper of mpRPC C++ library for Python";

    add_logging(module);
}
