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
 * \brief implementation of spdlog_logger class
 */
#include "mprpc/logging/spdlog_logger.h"

#include <spdlog/logger.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace mprpc {
namespace logging {

namespace impl {

/*!
 * \brief convert log level to spdlog library's one
 *
 * \param level log level
 * \return log level in spdlog library
 */
inline spdlog::level::level_enum convert_log_level_to_spdlog(log_level level) {
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

/*!
 * \brief class to format log level
 */
class log_level_flag final : public spdlog::custom_flag_formatter {
public:
    /*!
     * \brief format log level
     *
     * \param msg log message
     * \param dest destination
     */
    void format(const spdlog::details::log_msg& msg, const std::tm& /*time*/,
        spdlog::memory_buf_t& dest) override {
        // NOLINTNEXTLINE(clang-diagnostic-switch): prevent dead code
        switch (msg.level) {
        case spdlog::level::trace:
            format_impl(dest, "[trace]");
            break;
        case spdlog::level::debug:
            format_impl(dest, "[debug]");
            break;
        case spdlog::level::info:
            format_impl(dest, "[info] ");
            break;
        case spdlog::level::warn:
            format_impl(dest, "[warn] ");
            break;
        case spdlog::level::err:
            format_impl(dest, "[error]");
            break;
        case spdlog::level::critical:
            format_impl(dest, "[fatal]");
            break;
        }
    }

    /*!
     * \brief clone this
     *
     * \return cloned object
     */
    std::unique_ptr<custom_flag_formatter> clone() const override {
        return spdlog::details::make_unique<log_level_flag>();
    }

private:
    /*!
     * \brief format log level
     *
     * \param dest destination
     * \param str log level string
     */
    static void format_impl(spdlog::memory_buf_t& dest, const char* str) {
        constexpr std::size_t size = 7;
        // NOLINTNEXTLINE: use of external library
        dest.append(str, str + size);
    }
};

}  // namespace impl

/*!
 * \brief class of logger using spdlog library
 */
class spdlog_logger final : public logger {
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

        auto formatter = std::make_unique<spdlog::pattern_formatter>();
        formatter->add_flag<impl::log_level_flag>('k');
        formatter->set_pattern("[%Y-%m-%d %H:%M:%S.%f] %^%k%$ (thread %t) %v");
        spdlog_logger_->set_formatter(std::move(formatter));
    }

    /*!
     * \brief write log
     *
     * \param filename file name
     * \param line line number
     * \param level log level
     * \param message log message
     */
    void write_impl(const char* filename, std::uint32_t line,
        const char* /*function*/, log_level level,
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

std::shared_ptr<logger> create_spdlog_logger(
    std::shared_ptr<spdlog::logger> logger,
    mprpc::logging::log_level log_output_level) {
    return std::make_shared<spdlog_logger>(std::move(logger), log_output_level);
}

std::shared_ptr<logger> create_file_logger(const std::string& logger_name,
    const std::string& base_filename, log_level log_output_level,
    std::size_t max_file_size, std::size_t max_files, bool rotate_on_open) {
    return std::make_shared<spdlog_logger>(
        spdlog::rotating_logger_mt(logger_name, base_filename, max_file_size,
            max_files, rotate_on_open),
        log_output_level);
}

std::shared_ptr<logger> create_stdout_logger(log_level log_output_level) {
    static const auto spdlog_console_logger =
        spdlog::stdout_color_mt("mprpc_console_logger");
    return std::make_shared<spdlog_logger>(
        spdlog_console_logger, log_output_level);
}

}  // namespace logging
}  // namespace mprpc
