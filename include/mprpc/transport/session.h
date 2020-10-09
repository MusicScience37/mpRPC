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
 * \brief declaration and implementation of session class
 */
#pragma once

#include <functional>
#include <memory>

#include "mprpc/error_info.h"
#include "mprpc/message_data.h"
#include "mprpc/transport/address.h"

namespace mprpc {
namespace transport {

/*!
 * \brief base class of sessions
 */
class session {
public:
    //! type of handlers on reading a message
    using on_read_handler_type =
        std::function<void(const error_info&, const message_data&)>;

    //! type of handlers on writing a message
    using on_write_handler_type = std::function<void(const error_info&)>;

    /*!
     * \brief asynchronously read a message
     *
     * \param handler handler
     */
    virtual void async_read(on_read_handler_type handler) = 0;

    /*!
     * \brief asynchronously write a message
     *
     * \param data message data
     * \param handler handler
     */
    virtual void async_write(
        const message_data& data, on_write_handler_type handler) = 0;

    /*!
     * \brief get the address of remote endpoint
     *
     * \return address of remote endpoint
     */
    virtual std::shared_ptr<const address> remote_address() const = 0;

    //! construct
    session() noexcept = default;

    //! destruct
    virtual ~session() = default;

    session(const session&) = delete;
    session(session&&) = delete;
    session& operator=(const session&) = delete;
    session& operator=(session&&) = delete;
};

}  // namespace transport
}  // namespace mprpc
