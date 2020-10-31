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
 * \brief declaration and implementation of method_server class
 */
#pragma once

#include <functional>

#include <stl_ext/shared_function.h>

#include "mprpc/error_info.h"
#include "mprpc/message.h"
#include "mprpc/transport/session.h"

namespace mprpc {
namespace execution {

/*!
 * \brief class of servers to execute methods
 */
class method_server {
public:
    /*!
     * \brief type of handlers on message processed
     *
     * Parameters:
     *
     * - error
     * - whether response exists
     * - response message (if exists)
     */
    using on_message_processed_handler = stl_ext::shared_function<void(
        const error_info&, bool, const message_data&)>;

    /*!
     * \brief asynchronously process message
     *
     * \param session session
     * \param msg message
     * \param handler handler on message processed
     */
    virtual void async_process_message(
        const std::shared_ptr<transport::session>& session, const message& msg,
        on_message_processed_handler handler) = 0;

    //! construct
    method_server() noexcept = default;

    //! destruct
    virtual ~method_server() = default;

    method_server(const method_server&) = delete;
    method_server(method_server&&) = delete;
    method_server& operator=(const method_server&) = delete;
    method_server& operator=(method_server&&) = delete;
};

}  // namespace execution
}  // namespace mprpc
