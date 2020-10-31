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
 * \brief declaration and implementation of spdlog_logger class
 */
#pragma once

#include <spdlog/fwd.h>

#include "mprpc/logging/logger.h"
#include "mprpc/mprpc_export.h"

namespace mprpc {
namespace logging {

namespace impl {

//! default maximum size of a file
static constexpr std::size_t default_max_file_size = 1024 * 1024 * 10;

//! default maximum number of files
static constexpr std::size_t default_max_files = 10;

}  // namespace impl

/*!
 * \brief create logger with spdlog
 *
 * \param spdlog_logger logger in spdlog
 * \param log_output_level log output level
 * \return logger
 */
MPRPC_EXPORT std::shared_ptr<logger> create_spdlog_logger(
    std::shared_ptr<spdlog::logger> spdlog_logger,
    mprpc::logging::log_level log_output_level);

/*!
 * \brief create logger to write logs to files
 *
 * \param logger_name name of logger (unique in process)
 * \param base_filename base file name
 * \param log_output_level log output level
 * \param max_file_size maximum size of a file
 * \param max_files maximum number of files
 * \param rotate_on_open whether rotate file on opening this logger
 * \return logger
 */
MPRPC_EXPORT std::shared_ptr<logger> create_file_logger(
    const std::string& logger_name, const std::string& base_filename,
    log_level log_output_level = log_level::info,
    std::size_t max_file_size = impl::default_max_file_size,
    std::size_t max_files = impl::default_max_files,
    bool rotate_on_open = false);

/*!
 * \brief create logger to write logs to standard output
 *
 * \param log_output_level log output level
 * \return logger
 */
MPRPC_EXPORT std::shared_ptr<logger> create_stdout_logger(
    log_level log_output_level = log_level::info);

}  // namespace logging
}  // namespace mprpc
