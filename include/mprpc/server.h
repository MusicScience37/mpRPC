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
 * \brief declaration and implementation of server class
 */
#pragma once

#include <vector>

#include "mprpc/execution/method_server.h"
#include "mprpc/logging/logger.h"
#include "mprpc/thread_pool.h"
#include "mprpc/transport/acceptor.h"

namespace mprpc {

/*!
 * \brief class of servers
 */
class server {
public:
    /*!
     * \brief construct
     *
     * \param logger logger
     * \param threads thread pool
     * \param acceptors acceptors
     * \param method_server method server
     */
    server(std::shared_ptr<mprpc::logging::logger> logger,
        std::shared_ptr<mprpc::thread_pool> threads,
        std::vector<std::shared_ptr<transport::acceptor>> acceptors,
        std::shared_ptr<execution::method_server> method_server)
        : logger_(std::move(logger)),
          threads_(std::move(threads)),
          acceptors_(std::move(acceptors)),
          method_server_(std::move(method_server)) {}

    /*!
     * \brief start process
     */
    void start() {
        for (const auto& acceptor : acceptors_) {
            do_accept(acceptor);
        }
    }

    server(const server&) = delete;
    server& operator=(const server&) = delete;

    /*!
     * \brief copy
     */
    server(server&&) = default;

    /*!
     * \brief copy
     *
     * \return this object
     */
    server& operator=(server&&) = default;

    //! destruct
    ~server() = default;

private:
    /*!
     * \brief accept a session
     *
     * \param acceptor acceptor
     */
    void do_accept(const std::shared_ptr<transport::acceptor>& acceptor) {
        acceptor->async_accept(
            [this, acceptor](const error_info& error,
                const std::shared_ptr<transport::session>& session) {
                this->on_accept(acceptor, error, session);
            });
    }

    /*!
     * \brief process on session accepted
     *
     * \param acceptor acceptor
     * \param error error
     * \param session session
     */
    void on_accept(const std::shared_ptr<transport::acceptor>& acceptor,
        const error_info& error,
        const std::shared_ptr<transport::session>& session) {
        if (!error) {
            do_read(session);
        }
        do_accept(acceptor);
    }

    /*!
     * \brief read a message
     *
     * \param session session
     */
    void do_read(const std::shared_ptr<transport::session>& session) {
        session->async_read(
            [this, session](const error_info& error, const message_data& data) {
                this->on_read(session, error, data);
            });
    }

    /*!
     * \brief process on message read
     *
     * \param session session
     * \param error error
     * \param data message data
     */
    void on_read(const std::shared_ptr<transport::session>& session,
        const error_info& error, const message_data& data) {
        if (error) {
            return;
        }
        try {
            const auto msg = message(data);
            method_server_->async_process_message(session, msg,
                [session](const error_info& error, bool response_exists,
                    const message_data& response) {
                    on_execution(session, error, response_exists, response);
                });
        } catch (const exception& e) {
            MPRPC_ERROR(logger_, "{}", e.info());
            return;
        }
        do_read(session);
    }

    /*!
     * \brief process on execution of methods
     *
     * \param session session
     * \param error error
     * \param response_exists whether response exists
     * \param response response
     */
    static void on_execution(const std::shared_ptr<transport::session>& session,
        const error_info& error, bool response_exists,
        const message_data& response) {
        if (error) {
            return;
        }
        if (!response_exists) {
            return;
        }
        session->async_write(response, [](const error_info& /*error*/) {});
    }

    //! logger
    std::shared_ptr<mprpc::logging::logger> logger_;

    //! thread pool
    std::shared_ptr<mprpc::thread_pool> threads_;

    //! acceptors
    std::vector<std::shared_ptr<transport::acceptor>> acceptors_;

    //! method server
    std::shared_ptr<execution::method_server> method_server_;
};

}  // namespace mprpc
