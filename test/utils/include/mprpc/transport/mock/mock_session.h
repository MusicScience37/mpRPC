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

#include <mutex>

#include <asio.hpp>

#include "mprpc/transport/mock/mock_address.h"
#include "mprpc/transport/session.h"

namespace mprpc {
namespace transport {
namespace mock {

/*!
 * \brief class to mock sessions
 */
class mock_session final : public session {
public:
    /*!
     * \brief construct
     *
     * \param context context
     */
    explicit mock_session(asio::io_context& context) : context_(context) {}

    //! \copydoc mprpc::transport::session::async_read
    void async_read(on_read_handler_type handler) final {
        std::unique_lock<std::mutex> lock(mutex_);
        on_read_ = std::move(handler);
    }

    /*!
     * \brief add data to be read
     *
     * \note Call this after async_read, or nothing happens.
     *
     * \param data data
     */
    void add_read_data(const message_data& data) {
        std::unique_lock<std::mutex> lock(mutex_);
        on_read_handler_type on_read = std::move(on_read_);
        lock.unlock();
        if (on_read) {
            asio::post(context_, [on_read, data_copy = data] {
                on_read(error_info(), data_copy);
            });
        }
    }

    //! \copydoc mprpc::transport::session::async_write
    void async_write(
        const message_data& data, on_write_handler_type handler) final {
        std::unique_lock<std::mutex> lock(mutex_);
        written_data_ = data;
        lock.unlock();
        asio::post(context_, [handler] { handler(error_info()); });
    }

    /*!
     * \brief get the last written data
     *
     * \return last written data
     */
    message_data get_written_data() {
        std::unique_lock<std::mutex> lock(mutex_);
        message_data data = written_data_;
        return data;
    }

    //! \copydoc mprpc::transport::connector::remote_address
    std::shared_ptr<const address> remote_address() const override {
        return std::make_shared<mock_address>();
    }

private:
    //! context
    asio::io_context& context_;

    //! mutex
    std::mutex mutex_{};

    //! handler on reading a message
    on_read_handler_type on_read_{};

    //! last written data
    message_data written_data_{"", 0};
};

}  // namespace mock
}  // namespace transport
}  // namespace mprpc
