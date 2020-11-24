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
 * \brief declaration and implementation of udp_session class
 */
#pragma once

#include "mprpc/require_nonull.h"
#include "mprpc/transport/session.h"
#include "mprpc/transport/udp/impl/udp_address.h"
#include "mprpc/transport/udp/impl/udp_common.h"

namespace mprpc {
namespace transport {
namespace udp {
namespace impl {

/*!
 * \brief class of sessions on UDP sockets
 *
 * \note UDP has no session, but this class performs compatible process.
 */
class udp_session final : public session {
public:
    /*!
     * \brief construct
     *
     * \param logger logger
     * \param helper socket helper
     * \param endpoint endpoint
     * \param data data
     */
    udp_session(const logging::labeled_logger& logger,
        std::shared_ptr<udp_common> helper, asio::ip::udp::endpoint endpoint,
        message_data data)
        : helper_(MPRPC_REQUIRE_NONULL_MOVE(helper)),
          endpoint_(std::move(endpoint)),
          data_(std::move(data)) {
        MPRPC_INFO(logger, "accepted connection from {} at {}", endpoint,
            helper_->socket().local_endpoint());
    }

    //! \copydoc mprpc::transport::session::async_read
    void async_read(on_read_handler_type handler) override {
        if (data_.size() == 0) {
            handler(error_info(error_code::failed_to_read,
                        "second call to "
                        "mprpc::transport::udp::udp_session::async_read"),
                message_data());
        } else {
            const message_data data = data_;
            data_ = message_data();
            handler(error_info(), data);
        }
    }

    //! \copydoc mprpc::transport::session::async_write
    void async_write(
        const message_data& data, on_write_handler_type handler) override {
        helper_->async_write(endpoint_, data, handler);
    }

    //! \copydoc mprpc::transport::session::remote_address
    std::shared_ptr<const address> remote_address() const override {
        return std::make_shared<udp_address>(endpoint_);
    }

private:
    //! socket helper
    std::shared_ptr<udp_common> helper_;

    //! endpoint
    asio::ip::udp::endpoint endpoint_;

    //! message
    message_data data_;
};

}  // namespace impl
}  // namespace udp
}  // namespace transport
}  // namespace mprpc
