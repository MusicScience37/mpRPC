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

#include <spdlog/logger.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "mprpc/logging/logger.h"

namespace mprpc {
namespace logging {

namespace impl {

/*!
 * \brief convert log level to spdlog library's one
 *
 * \param level log level
 * \return log level in spdlog library
 */
spdlog::level::level_enum convert_log_level_to_spdlog(log_level level) {
    switch (level) {
    case log_level::trace:
        return spdlog::level::trace;
    case log_level::debug:
        return spdlog::level::debug;
    case log_level::info:
        return spdlog::level::info;
    case log_level::warn:
        return spdlog::level::warn;
    case log_level::error:
        return spdlog::level::err;
    case log_level::fatal:
        return spdlog::level::critical;
    default:
        return spdlog::level::trace;
    }
}

}  // namespace impl

/*!
 * \brief class of logger using spdlog library
 */
class spdlog_logger : public logger {
public:
    /*!
     * \brief construct
     *
     * \param spdlog_logger logger in spdlog
     * \param log_output_level log output level
     */
    spdlog_logger(std::shared_ptr<spdlog::logger> spdlog_logger,
        mprpc::logging::log_level log_output_level) noexcept
        : logger(log_output_level), spdlog_logger_(std::move(spdlog_logger)) {
        spdlog_logger_->set_level(spdlog::level::trace);
        spdlog_logger_->flush_on(spdlog::level::trace);
        spdlog_logger_->set_pattern(
            "[%Y-%m-%d %H:%M:%S.%f] %^[%l]%$ (thread %t) %v");
    }

    /*!
     * \brief get internal logger in spdlog
     *
     * \return logger in spdlog
     */
    std::shared_ptr<spdlog::logger> internal_logger() const noexcept {
        return spdlog_logger_;
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
        const char* function, log_level level,
        const char* message) noexcept final {
        write_impl(level,
            fmt::format(FMT_STRING("{} ({}:{})"), message, filename, line)
                .c_str());
    }

    /*!
     * \brief write log
     *
     * \param level log level
     * \param message log message
     */
    void write_impl(log_level level, const char* message) noexcept final {
        spdlog_logger_->log(impl::convert_log_level_to_spdlog(level), message);
    }

private:
    //! logger in spdlog
    std::shared_ptr<spdlog::logger> spdlog_logger_;
};

namespace impl {

//! default maximum size of a file
static constexpr std::size_t default_max_file_size = 1024 * 1024 * 10;

//! default maximum number of files
static constexpr std::size_t default_max_files = 10;

}  // namespace impl

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
std::shared_ptr<spdlog_logger> create_file_logger(
    const std::string& logger_name, const std::string& base_filename,
    log_level log_output_level = log_level::info,
    std::size_t max_file_size = impl::default_max_file_size,
    std::size_t max_files = impl::default_max_files,
    bool rotate_on_open = false) {
    return std::make_shared<spdlog_logger>(
        spdlog::rotating_logger_mt(logger_name, base_filename, max_file_size,
            max_files, rotate_on_open),
        log_output_level);
}

/*!
 * \brief create logger to write logs to standard output
 *
 * \param logger_name name of logger (unique in process)
 * \param log_output_level log output level
 * \return logger
 */
std::shared_ptr<spdlog_logger> create_stdout_logger(
    const std::string& logger_name,
    log_level log_output_level = log_level::info) {
    return std::make_shared<spdlog_logger>(
        spdlog::stdout_color_mt(logger_name), log_output_level);
}

}  // namespace logging
}  // namespace mprpc
