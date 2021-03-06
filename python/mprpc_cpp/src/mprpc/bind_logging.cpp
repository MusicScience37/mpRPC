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
 * \brief implementation of bind_logging function
 */
#include "mprpc/bind_logging.h"

#include <pybind11/functional.h>

#include "mprpc/logging/basic_loggers.h"
#include "mprpc/logging/log_level.h"
#include "mprpc/logging/logger.h"
#include "mprpc/python_logger_helper.h"

namespace mprpc {
namespace python {

void bind_logging(pybind11::module& module) {
    using mprpc::logging::log_level;
    pybind11::enum_<log_level>(module, "LogLevel", "enumeration of log levels")
        .value("TRACE", log_level::trace, "trace")
        .value("DEBUG", log_level::debug, "debug")
        .value("INFO", log_level::info, "information")
        .value("WARN", log_level::warn, "warning")
        .value("ERROR", log_level::error, "error")
        .value("FATAL", log_level::fatal, "fatal")
        .value("NONE", log_level::none, "none (for log output level)")
        .export_values();

    using mprpc::logging::logger;
    // NOLINTNEXTLINE: no member to export
    pybind11::class_<logger, std::shared_ptr<logger>>(
        module, "Logger", "base class of loggers")
        .def(
            "write",
            [](const std::shared_ptr<logger>& self, const std::string& label,
                const std::string& filename, std::uint32_t line,
                const std::string& function, log_level level,
                const std::string& message) {
                self->write(label.c_str(), filename.c_str(), line,
                    function.c_str(), level, message.c_str());
            },
            R"doc(write(self, label : str, filename : str, line : int, function : str, log_level : LogLevel, message : str)

            write log

            Parameters
            ----------
            label : str
                label
            filename : str
                file name
            line : int
                line number
            function : str
                function name
            log_level : mprpc.logging.LogLevel
                log level
            message : str
                log message

            Returns
            -------
            None
                none
            )doc");

    using mprpc::python::python_logger_helper;
    pybind11::class_<python_logger_helper, logger,
        std::shared_ptr<python_logger_helper>>(module, "PythonLoggerHelper",
        R"doc(
            class of loggers for implementation in Python

            Parameters
            ----------
            log_writer : Callable
                function to write logs with signature same as mprpc.logging.Logger.write function.
            log_output_level : mprpc.logging.LogLevel
                log output level
        )doc")
        .def(pybind11::init<
            const std::function<void(const std::string&, const std::string&,
                std::uint32_t, const std::string&, log_level,
                const std::string&)>&,
            log_level>());

    using mprpc::logging::create_file_logger;
    module.def("create_file_logger", &create_file_logger,
        pybind11::arg("base_filename"),
        pybind11::arg("log_output_level") = log_level::warn,
        pybind11::arg("max_file_size") = mprpc::logging::default_max_file_size,
        pybind11::arg("max_files") = mprpc::logging::default_max_files,
        pybind11::arg("rotate_on_open") = false,
        R"doc(create_file_logger(base_filename : str, log_output_level : LogLevel =LogLevel.WARN, max_file_size : int =1024*1024*10, max_files : int =10, rotate_on_open : bool =False) -> Logger

            create a logger writing to files

            Parameters
            ----------
            base_filename : str
                base file name
            log_output_level : mprpc.logging.LogLevel, optional
                log output level
            max_file_size : int, optional
                maximum size of a file
            max_files : int, optional
                maximum number of files
            rotate_on_open : bool, optional
                whether rotate file on opening this logger

            Returns
            -------
            mprpc.logging.Logger
                logger
        )doc");

    using mprpc::logging::create_stdout_logger;
    module.def("create_stdout_logger", &create_stdout_logger,
        pybind11::arg("log_output_level") = log_level::warn,
        R"doc(create_stdout_logger(log_output_level : LogLevel =LogLevel.WARN)

            create a logger writing to standard output

            Parameters
            ----------
            log_output_level : mprpc.logging.LogLevel, optional
                log output level

            Returns
            -------
            mprpc.logging.Logger
                logger
        )doc");
}

}  // namespace python
}  // namespace mprpc
