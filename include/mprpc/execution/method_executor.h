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
 * \brief declaration and implementation of method_executor class
 */
#pragma once

#include <string>

#include "mprpc/message.h"
#include "mprpc/transport/session.h"

namespace mprpc {
namespace execution {

/*!
 * \brief base class of executors of methods
 */
class method_executor {
public:
    /*!
     * \brief get method name
     *
     * \return method name
     */
    virtual std::string name() const = 0;

    /*!
     * \brief process a request
     *
     * \param session session
     * \param msg message
     * \return response data
     */
    virtual message_data process_request(
        const std::shared_ptr<transport::session>& session,
        const message& msg) = 0;

    /*!
     * \brief process a notification
     *
     * \param session session
     * \param msg message
     */
    virtual void process_notification(
        const std::shared_ptr<transport::session>& session,
        const message& msg) = 0;

    //! construct
    method_executor() noexcept = default;

    //! destruct
    virtual ~method_executor() = default;

    method_executor(const method_executor&) = delete;
    method_executor(method_executor&&) = delete;
    method_executor& operator=(const method_executor&) = delete;
    method_executor& operator=(method_executor&&) = delete;
};

}  // namespace execution
}  // namespace mprpc
