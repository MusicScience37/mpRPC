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
 * \brief declaration and implementation of labeled_logger class
 */
#pragma once

#include <memory>
#include <utility>

#include "mprpc/logging/logger.h"

namespace mprpc {
namespace logging {

/*!
 * \brief class of logger with labels
 */
class labeled_logger {
public:
    /*!
     * \brief construct
     *
     * \param logger logger
     * \param label label
     */
    explicit labeled_logger(
        std::shared_ptr<logger> logger, std::string label = std::string())
        : logger_(std::move(logger)), label_(std::move(label)) {}

    /*!
     * \brief construct
     *
     * \param logger logger
     * \param name additional level name
     */
    labeled_logger(labeled_logger logger, const std::string& name)
        : labeled_logger(std::move(logger)) {
        add_level(name);
    }

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
        log_level level, Format&& format, Data&&... data) const noexcept {
        if (label_.empty()) {
            logger_->write(filename, line, function, level,
                std::forward<Format>(format), std::forward<Data>(data)...);
        } else {
            logger_->write(label_.c_str(), filename, line, function, level,
                std::forward<Format>(format), std::forward<Data>(data)...);
        }
    }

private:
    /*!
     * \brief add a level of the label
     *
     * \param name level name
     */
    void add_level(const std::string& name) {
        if (label_.empty()) {
            label_ = name;
        } else {
            label_ += '.';
            label_ += name;
        }
    }

    //! logger
    std::shared_ptr<logger> logger_;

    //! label
    std::string label_;
};

}  // namespace logging
}  // namespace mprpc
