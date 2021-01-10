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
 * \brief declaration and implementation of server_base class
 */
#pragma once

#include <mutex>
#include <unordered_set>
#include <vector>

#include "mprpc/error_info.h"
#include "mprpc/execution/method_server.h"
#include "mprpc/logging/labeled_logger.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/require_nonull.h"
#include "mprpc/server_config.h"
#include "mprpc/thread_pool.h"
#include "mprpc/transport/acceptor.h"

namespace mprpc {
namespace impl {

/*!
 * \brief base class of servers
 *
 * This class implements message transport and session management.
 */
class server_base {
public:
    /*!
     * \brief construct
     *
     * \param logger logger
     * \param threads thread pool
     * \param acceptors acceptors
     * \param config configuration
     */
    server_base(logging::labeled_logger logger,
        std::shared_ptr<mprpc::thread_pool> threads,
        std::vector<std::shared_ptr<transport::acceptor>> acceptors,
        server_config config)
        : logger_(std::move(logger)),
          threads_(MPRPC_REQUIRE_NONULL_MOVE(threads)),
          acceptors_(std::move(acceptors)),
          config_(std::move(config)) {
        for (const auto& acceptor : acceptors_) {
            MPRPC_REQUIRE_NONULL(acceptor);
        }
    }

    /*!
     * \brief start process
     */
    void start() {
        MPRPC_INFO(logger_, "server starting");
        threads_->start();
        for (const auto& acceptor : acceptors_) {
            do_accept(acceptor);
        }
        MPRPC_INFO(logger_, "server started");
    }

    /*!
     * \brief stop process
     */
    void stop() {
        MPRPC_INFO(logger_, "server stopping");
        std::unique_lock<std::mutex> lock(*mutex_);
        for (const auto& session : sessions_) {
            session->shutdown();
        }
        lock.unlock();
        threads_->stop();
        MPRPC_INFO(logger_, "server stopped");
    }

    server_base(const server_base&) = delete;
    server_base& operator=(const server_base&) = delete;
    server_base(server_base&&) = delete;
    server_base& operator=(server_base&&) = delete;

    //! destruct
    ~server_base() { stop(); }

protected:
    /*!
     * \brief asynchronously process message
     *
     * \param session session
     * \param data message data
     * \param handler handler on message processed
     */
    virtual void async_process_message(
        const std::shared_ptr<transport::session>& session,
        const message_data& data,
        const execution::method_server::on_message_processed_handler&
            handler) = 0;

    /*!
     * \brief asynchronously remove a session
     *
     * \param session session
     */
    void async_remove_session(
        const std::shared_ptr<transport::session>& session) {
        threads_->post([this, session] {
            std::unique_lock<std::mutex> lock(*mutex_);
            session->shutdown();
            sessions_.erase(session);
        });
    }

private:
    /*!
     * \brief accept a session
     *
     * \param acceptor acceptor
     */
    void do_accept(const std::shared_ptr<transport::acceptor>& acceptor) {
        acceptor->async_accept(
            [this,
                weak_acceptor = std::weak_ptr<transport::acceptor>(acceptor)](
                const error_info& error,
                const std::shared_ptr<transport::session>& session) {
                auto acceptor = weak_acceptor.lock();
                if (acceptor) {
                    this->on_accept(acceptor, error, session);
                }
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
            std::unique_lock<std::mutex> lock(*mutex_);
            sessions_.insert(session);
            lock.unlock();
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
            [this, weak_session = std::weak_ptr<transport::session>(session)](
                const error_info& error, const message_data& data) {
                auto session = weak_session.lock();
                if (session) {
                    this->on_read(session, error, data);
                }
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
            async_remove_session(session);
            return;
        }
        async_process_message(session, data,
            [session](const error_info& error, bool response_exists,
                const message_data& response) {
                on_execution(session, error, response_exists, response);
            });

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
    logging::labeled_logger logger_;

    //! thread pool
    std::shared_ptr<mprpc::thread_pool> threads_;

    //! acceptors
    std::vector<std::shared_ptr<transport::acceptor>> acceptors_;

    //! session
    std::unordered_set<std::shared_ptr<transport::session>> sessions_{};

    //! mutex for session
    std::unique_ptr<std::mutex> mutex_{std::make_unique<std::mutex>()};

    //! server configuration
    const server_config config_;
};

}  // namespace impl
}  // namespace mprpc
