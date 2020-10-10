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
 * \brief declaration and implementation of message class
 */
#pragma once

#include <memory>
#include <string>

#include <msgpack.hpp>

#include "mprpc/exception.h"
#include "mprpc/message_def.h"
#include "mprpc/pack_data.h"

namespace mprpc {

/*!
 * \brief class of message
 */
class message {
public:
    //! construct
    message() = default;

    /*!
     * \brief construct
     *
     * \param handle handle of parsed object
     */
    explicit message(msgpack::object_handle handle) {
        parse(std::move(handle));
    }

    /*!
     * \brief construct
     *
     * \param data message data
     */
    explicit message(const message_data& data) {
        try {
            parse(msgpack::unpack(data.data(), data.size()));
        } catch (const msgpack::parse_error&) {
            throw exception(
                error_info(error_code::parse_error, "parse error of message"));
        }
    }

    /*!
     * \brief get message type
     *
     * \return message type
     */
    msgtype type() const noexcept { return data_->type; }

    /*!
     * \brief get message ID
     *
     * \return message ID
     */
    msgid_t msgid() const noexcept { return data_->msgid; }

    /*!
     * \brief get method name
     *
     * \return method name
     */
    const std::string& method() const {
        if (data_->type == msgtype::response) {
            throw exception(error_info(error_code::invalid_message,
                "tried to access to method name of response message",
                pack_data(data_->handle.get())));
        }
        return data_->method;
    }

    /*!
     * \brief get parameters
     *
     * \return parameters
     */
    const msgpack::object& params() const {
        if (data_->type == msgtype::response) {
            throw exception(error_info(error_code::invalid_message,
                "tried to access to parameters of response message",
                pack_data(data_->handle.get())));
        }
        return *(data_->params);
    }

    /*!
     * \brief get parameters as a type
     *
     * \tparam T type to convert to
     * \return data
     */
    template <typename T>
    T params_as() const {
        try {
            return params().as<T>();
        } catch (const msgpack::type_error&) {
            throw exception(error_info(error_code::invalid_message,
                "failed to convert parameters",
                pack_data(data_->handle.get())));
        }
    }

private:
    /*!
     * \brief parse an object
     *
     * \param handle handle of an object
     */
    void parse(msgpack::object_handle handle) {
        data_->handle = std::move(handle);

        if (data_->handle->type != msgpack::type::ARRAY) {
            throw exception(error_info(error_code::invalid_message,
                "message is not an array", pack_data(data_->handle.get())));
        }
        // NOLINTNEXTLINE: use of external library
        const auto& array = data_->handle->via.array;

        if (array.size < 3) {
            throw exception(error_info(error_code::invalid_message,
                "message must have at least 3 elements",
                pack_data(data_->handle.get())));
        }

        try {
            data_->type = array.ptr[0].as<msgtype>();
        } catch (const msgpack::type_error&) {
            throw exception(error_info(error_code::invalid_message,
                "message type is not an integer",
                pack_data(data_->handle.get())));
        }

        switch (data_->type) {
        case msgtype::request:
            parse_request();
            break;
        case msgtype::response:
            parse_response();
            break;
        case msgtype::notification:
            parse_notification();
            break;
        default:
            throw exception(error_info(error_code::invalid_message,
                "invalid message type", pack_data(data_->handle.get())));
        }
    }

    //! parse request
    void parse_request() {
        // NOLINTNEXTLINE: use of external library
        const auto& array = data_->handle->via.array;

        if (array.size != 4) {
            throw exception(error_info(error_code::invalid_message,
                "request message must have 4 elements",
                pack_data(data_->handle.get())));
        }

        try {
            data_->msgid = array.ptr[1].as<msgid_t>();
        } catch (const msgpack::type_error&) {
            throw exception(error_info(error_code::invalid_message,
                "message ID must a 32-but unsigned integer",
                pack_data(data_->handle.get())));
        }

        try {
            data_->method = array.ptr[2].as<std::string>();
        } catch (const msgpack::type_error&) {
            throw exception(error_info(error_code::invalid_message,
                "method name must be a string",
                pack_data(data_->handle.get())));
        }

        data_->params = array.ptr + 3;
        if (data_->params->type != msgpack::type::ARRAY) {
            throw exception(error_info(error_code::invalid_message,
                "paramters must be an array", pack_data(data_->handle.get())));
        }
    }

    //! parse response
    void parse_response() {
        //
    }

    //! parse notification
    void parse_notification() {
        //
    }

    //! struct of data
    struct inner_data {
        //! handle of parsed object
        msgpack::object_handle handle{};

        //! message type
        msgtype type{static_cast<msgtype>(-1)};

        //! message ID
        msgid_t msgid{0};

        //! method name
        std::string method{};

        //! parameters
        msgpack::object* params{nullptr};
    };

    //! data
    std::shared_ptr<inner_data> data_{std::make_shared<inner_data>()};
};

}  // namespace mprpc
