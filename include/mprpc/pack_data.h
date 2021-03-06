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
 * \brief declaration and implementation of pack_data function
 */
#pragma once

#include <tuple>

#include <msgpack.hpp>

#include "mprpc/message_data.h"
#include "mprpc/message_def.h"

namespace mprpc {

/*!
 * \brief pack data
 *
 * \tparam T data type
 * \param data data
 * \return packed data
 */
template <typename T>
inline message_data pack_data(const T& data) {
    msgpack::sbuffer buf;
    msgpack::pack(buf, data);
    return message_data(buf.data(), buf.size());
}

/*!
 * \brief pack request
 *
 * \tparam Params type of parameters
 * \param id message ID
 * \param method method name
 * \param params parameters
 * \return message data
 */
template <typename Params>
inline message_data pack_request(
    msgid_t id, const std::string& method, const Params& params) {
    return pack_data(
        std::forward_as_tuple(msgtype::request, id, method, params));
}

/*!
 * \brief pack response
 *
 * \tparam Result type of result
 * \param id message ID
 * \param result result
 * \return message data
 */
template <typename Result>
inline message_data pack_response(msgid_t id, const Result& result) {
    return pack_data(std::forward_as_tuple(
        msgtype::response, id, msgpack::type::nil_t(), result));
}

/*!
 * \brief pack response with error
 *
 * \tparam Error type of error data
 * \param id message ID
 * \param error error data
 * \return message data
 */
template <typename Error>
inline message_data pack_error_response(msgid_t id, const Error& error) {
    return pack_data(std::forward_as_tuple(
        msgtype::response, id, error, msgpack::type::nil_t()));
}

/*!
 * \brief pack notification
 *
 * \tparam Params type of parameters
 * \param method method name
 * \param params parameters
 * \return message data
 */
template <typename Params>
inline message_data pack_notification(
    const std::string& method, const Params& params) {
    return pack_data(
        std::forward_as_tuple(msgtype::notification, method, params));
}

}  // namespace mprpc
