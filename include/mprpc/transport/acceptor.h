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
 * \brief declaration and implementation of acceptor class
 */
#pragma once

#include <functional>
#include <memory>

#include "mprpc/error_info.h"
#include "mprpc/transport/session.h"

namespace mprpc {
namespace transport {

/*!
 * \brief base class of acceptors
 */
class acceptor {
public:
    //! type of handlers on accepting connections
    using on_accept_handler_type =
        std::function<void(const error_info&, std::shared_ptr<session>)>;

    /*!
     * \brief asynchronously accept a connection
     *
     * \param handler handler
     */
    virtual void async_accept(on_accept_handler_type handler) = 0;

    //! construct
    acceptor() noexcept = default;

    //! destruct
    virtual ~acceptor() = default;

    acceptor(const acceptor&) = delete;
    acceptor(acceptor&&) = delete;
    acceptor& operator=(const acceptor&) = delete;
    acceptor& operator=(acceptor&&) = delete;
};

}  // namespace transport
}  // namespace mprpc
