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
 * \brief declaration and implementation of logger class
 */
#pragma once

#include <array>
#include <cstdint>
#include <exception>

#include "mprpc/logging/format_log_data.h"
#include "mprpc/logging/log_level.h"

namespace mprpc {
namespace logging {

/*!
 * \brief base class of loggers
 */
class logger {
public:
    /*!
     * \brief write log
     *
     * \note if no data given, format variable will be treated as log message.
     *
     * \tparam Format format type
     * \tparam Data data type
     * \param filename file name
     * \param line line number
     * \param function function name
     * \param level log level
     * \param format format
     * \param data data
     */
    template <typename Format, typename... Data>
    void write(const char* filename, std::uint32_t line, const char* function,
        log_level level, Format&& format, Data&&... data) noexcept {
        if (!is_outputted_level(level)) {
            return;
        }
        try {
            write_impl(filename, line, function, level,
                format_log_data(
                    std::forward<Format>(format), std::forward<Data>(data)...)
                    .c_str());
        } catch (...) {
            write_impl(filename, line, function, level,
                "ERROR IN FORMATTING LOG MESSAGE");
        }
    }

    /*!
     * \brief write log
     *
     * \note if no data given, format variable will be treated as log message.
     *
     * \tparam Format format type
     * \tparam Data data type
     * \param level log level
     * \param format format
     * \param data data
     */
    template <typename Format, typename... Data>
    void write(log_level level, Format&& format, Data&&... data) noexcept {
        if (!is_outputted_level(level)) {
            return;
        }
        try {
            write_impl(level,
                format_log_data(
                    std::forward<Format>(format), std::forward<Data>(data)...)
                    .c_str());
        } catch (...) {
            write_impl(level, "ERROR IN FORMATTING LOG MESSAGE");
        }
    }

    /*!
     * \brief get log output level
     *
     * \return log output level
     */
    log_level log_output_level() const noexcept { return log_output_level_; }

    /*!
     * \brief check whether a log with given log level will be outputted
     *
     * \param level log level
     * \return whether a log with given log level will be outputted
     */
    bool is_outputted_level(log_level level) const noexcept {
        return static_cast<int>(level) >= static_cast<int>(log_output_level_);
    }

    logger(const logger&) = delete;
    logger(logger&&) = delete;
    logger& operator=(const logger&) = delete;
    logger& operator=(logger&&) = delete;

    //! destruct
    virtual ~logger() = default;

protected:
    /*!
     * \brief construct
     *
     * \param log_output_level log output level
     */
    explicit logger(log_level log_output_level) noexcept
        : log_output_level_(log_output_level) {}

    /*!
     * \brief write log
     *
     * \param filename file name
     * \param line line number
     * \param function function name
     * \param level log level
     * \param message log message
     */
    virtual void write_impl(const char* filename, std::uint32_t line,
        const char* function, log_level level,
        const char* message) noexcept = 0;

    /*!
     * \brief write log
     *
     * \param level log level
     * \param message log message
     */
    virtual void write_impl(log_level level, const char* message) noexcept = 0;

private:
    //! log output level
    log_level log_output_level_;
};

}  // namespace logging
}  // namespace mprpc
