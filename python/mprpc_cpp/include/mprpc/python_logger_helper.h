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
 * \brief declaration and implementation of python_logger_helper class
 */
#pragma once

#include <functional>
#include <iostream>

#include "mprpc/logging/log_level.h"
#include "mprpc/logging/logger.h"

namespace mprpc {
namespace python {

/*!
 * \brief class of loggers for implementation in Python
 */
class python_logger_helper final : public logging::logger {
public:
    /*!
     * \brief construct
     *
     * \param write_func function to write logs
     * \param log_output_level log output level
     */
    explicit python_logger_helper(
        std::function<void(const std::string&, const std::string&,
            std::uint32_t, const std::string&, logging::log_level,
            const std::string&)>
            write_func,
        logging::log_level log_output_level)
        : logging::logger(log_output_level),
          write_func_(std::move(write_func)) {}

    /*!
     * \brief write log
     *
     * \param label label
     * \param filename file name
     * \param line line number
     * \param function function name
     * \param level log level
     * \param message log message
     */
    void write_impl(const char* label, const char* filename, std::uint32_t line,
        const char* function, logging::log_level level,
        const char* message) noexcept override {
        try {
            write_func_(label, filename, line, function, level, message);
        } catch (const std::exception& e) {
            std::cerr << "Error in mprpc::python_logger: " << e.what()
                      << std::endl;
        }
    }

    /*!
     * \brief write log
     *
     * \param filename file name
     * \param line line number
     * \param function function name
     * \param level log level
     * \param message log message
     */
    void write_impl(const char* filename, std::uint32_t line,
        const char* function, logging::log_level level,
        const char* message) noexcept override {
        write_impl("mprpc", filename, line, function, level, message);
    }

    /*!
     * \brief write log
     *
     * \param level log level
     * \param message log message
     */
    void write_impl(
        logging::log_level level, const char* message) noexcept override {
        write_impl(
            "mprpc", "<unknown_file>", 0, "<unknown_function>", level, message);
    }

private:
    //! function to write log
    std::function<void(const std::string&, const std::string&, std::uint32_t,
        const std::string&, logging::log_level, const std::string&)>
        write_func_;
};

}  // namespace python
}  // namespace mprpc
