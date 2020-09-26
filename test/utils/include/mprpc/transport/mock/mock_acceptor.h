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
 * \brief declaration and implementation of mock_acceptor class
 */
#pragma once

#include <mutex>

#include <asio.hpp>

#include "mprpc/transport/acceptor.h"
#include "mprpc/transport/mock/mock_address.h"
#include "mprpc/transport/mock/mock_session.h"

namespace mprpc {
namespace transport {
namespace mock {

/*!
 * \brief class to mock acceptors
 */
class mock_acceptor : public acceptor {
public:
    /*!
     * \brief construct
     *
     * \param context context
     */
    explicit mock_acceptor(asio::io_context& context) : context_(context) {}

    //! \copydoc mprpc::transport::acceptor::async_accept
    void async_accept(on_accept_handler_type handler) final {
        std::unique_lock<std::mutex> lock(mutex_);
        on_accept_ = std::move(handler);
    }

    /*!
     * \brief create a session
     *
     * \note Call after async_accept.
     *
     * \return session
     */
    std::shared_ptr<mock_session> create_session() {
        auto session = std::make_shared<mock_session>(context_);
        std::unique_lock<std::mutex> lock(mutex_);
        on_accept_handler_type handler = std::move(on_accept_);
        lock.unlock();
        if (handler) {
            asio::post(context_,
                [handler, session] { handler(error_info(), session); });
        }
        return session;
    }

    //! \copydoc mprpc::transport::acceptor::local_address
    std::shared_ptr<address> local_address() const override {
        return std::make_shared<mock_address>();
    }

private:
    //! context
    asio::io_context& context_;

    //! mutex
    std::mutex mutex_{};

    //! handle on accepting connections
    on_accept_handler_type on_accept_{};
};

}  // namespace mock
}  // namespace transport
}  // namespace mprpc
