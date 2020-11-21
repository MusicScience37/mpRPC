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
 * \brief implementation of create_logger function
 */
#include "create_logger.h"

#include "mprpc/logging/basic_loggers.h"

std::shared_ptr<mprpc::logging::logger> create_logger_impl() {
    constexpr std::size_t max_file_size = 1024 * 1024;
    constexpr std::size_t max_files = 5;
    auto logger = mprpc::logging::create_file_logger("mprpc_test_units",
        "mprpc_test_units.log", mprpc::logging::log_level::trace, max_file_size,
        max_files, true);

    constexpr auto info_level = mprpc::logging::log_level::info;
    logger->write(info_level, "created a logger for unit tests");

    return logger;
}

std::shared_ptr<mprpc::logging::logger> create_logger(
    const std::string& test_name) {
    static auto logger = create_logger_impl();

    constexpr auto info_level = mprpc::logging::log_level::info;
    constexpr auto line_length = 120;
    const auto line = std::string(line_length, '-');
    logger->write(info_level, line);
    logger->write(info_level, "starting test {}", test_name);

    return logger;
}
