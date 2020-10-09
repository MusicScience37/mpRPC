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
 * \brief declaration and implementation of error_info class
 */
#pragma once

#include <memory>
#include <string>

#include <fmt/core.h>

#include "mprpc/error_code.h"
#include "mprpc/format_message.h"
#include "mprpc/logging/log_data_traits.h"
#include "mprpc/message_data.h"

namespace mprpc {

/*!
 * \brief class of error information
 */
class error_info {
public:
    //! construct (no error)
    error_info() noexcept = default;

    /*!
     * \brief construct
     *
     * \param code error code
     * \param message error message
     */
    error_info(error_code_t code, std::string message)
        : data_(std::make_shared<inner_data>(code, std::move(message))) {}

    /*!
     * \brief construct
     *
     * \param code error code
     * \param message error message
     * \param data data related to this error
     */
    error_info(error_code_t code, std::string message, message_data data)
        : data_(std::make_shared<inner_data>(
              code, std::move(message), std::move(data))) {}

    /*!
     * \brief get whether this has an error
     *
     * \return whether this has an error
     */
    bool has_error() const noexcept { return data_.operator bool(); }

    /*!
     * \brief get whether this has an error
     *
     * \return whether this has an error
     */
    explicit operator bool() const noexcept { return has_error(); }

    /*!
     * \brief get error code
     *
     * \return error code
     */
    error_code_t code() const noexcept {
        if (data_) {
            return data_->code();
        }
        return error_code::success;
    }

    /*!
     * \brief get error message
     *
     * \return error message
     */
    const std::string& message() const noexcept {
        if (data_) {
            return data_->message();
        }
        static const auto empty_data = std::string();
        return empty_data;
    }

    /*!
     * \brief get whether this has data related to this error
     *
     * \return whether this has data related to this error
     */
    bool has_data() const noexcept { return data_ && data_->data(); }

    /*!
     * \brief get data related to this error
     *
     * \return data related to this error
     */
    const message_data& data() const noexcept {
        if (data_ && data_->data()) {
            return *(data_->data());
        }
        static const auto empty_data = message_data();
        return empty_data;
    }

private:
    //! class of inner data
    class inner_data {
    public:
        /*!
         * \brief construct
         *
         * \param code error code
         * \param message error message
         */
        inner_data(error_code_t code, std::string&& message)
            : code_(code), message_(std::move(message)) {}

        /*!
         * \brief construct
         *
         * \param code error code
         * \param message error message
         * \param data data related to this error
         */
        inner_data(
            error_code_t code, std::string&& message, message_data&& data)
            : code_(code),
              message_(std::move(message)),
              data_(std::make_unique<message_data>(std::move(data))) {}

        /*!
         * \brief get error code
         *
         * \return error code
         */
        error_code_t code() const noexcept { return code_; }

        /*!
         * \brief get error message
         *
         * \return error message
         */
        const std::string& message() const noexcept { return message_; }

        /*!
         * \brief get data related to this error
         *
         * \return data related to this error
         */
        const std::unique_ptr<message_data>& data() const noexcept {
            return data_;
        }

    private:
        //! error code
        error_code_t code_;

        //! error message
        std::string message_;

        //! data related to this error
        std::unique_ptr<message_data> data_{};
    };

    //! inner data
    std::shared_ptr<inner_data> data_{};
};

namespace logging {

/*!
 * \brief log_data_traits for error_info class
 */
template <>
struct log_data_traits<error_info> {
    /*!
     * \brief preprocess data
     *
     * \param data data
     * \return formatted data
     */
    static std::string preprocess(const error_info& data) noexcept {
        if (!data) {
            return "no error";
        }
        if (!data.has_data()) {
            return fmt::format("error {} {}", data.code(), data.message());
        }
        return fmt::format("error {} {} with data: {}", data.code(),
            data.message(),
            log_data_traits<message_data>::preprocess(data.data()));
    }
};

}  // namespace logging

}  // namespace mprpc
