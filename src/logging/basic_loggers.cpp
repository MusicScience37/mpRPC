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
 * \brief implementation of factory methods of basic loggers
 */
#include "mprpc/logging/basic_loggers.h"

#include <spdlog/logger.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "mprpc/logging/impl/spdlog_logger.h"

namespace mprpc {
namespace logging {

std::shared_ptr<logger> create_file_logger(const std::string& logger_name,
    const std::string& base_filename, log_level log_output_level,
    std::size_t max_file_size, std::size_t max_files, bool rotate_on_open) {
    return std::make_shared<impl::spdlog_logger>(
        spdlog::rotating_logger_mt(logger_name, base_filename, max_file_size,
            max_files, rotate_on_open),
        log_output_level);
}

std::shared_ptr<logger> create_stdout_logger(log_level log_output_level) {
    static const auto spdlog_console_logger =
        spdlog::stdout_color_mt("mprpc_console_logger");
    return std::make_shared<impl::spdlog_logger>(
        spdlog_console_logger, log_output_level);
}

}  // namespace logging
}  // namespace mprpc
