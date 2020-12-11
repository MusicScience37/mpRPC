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

#include "mprpc/impl/server_base.h"
#include "mprpc/server_fwd.h"

namespace mprpc {

/*!
 * \brief class of servers
 */
class server final : public impl::server_base {
public:
    /*!
     * \brief construct
     *
     * \param logger logger
     * \param threads thread pool
     * \param acceptors acceptors
     * \param method_server method server
     * \param config configuration
     */
    server(const logging::labeled_logger& logger,
        std::shared_ptr<mprpc::thread_pool> threads,
        std::vector<std::shared_ptr<transport::acceptor>> acceptors,
        std::shared_ptr<execution::method_server> method_server,
        server_config config)
        : impl::server_base(logger, std::move(threads), std::move(acceptors),
              std::move(config)),
          logger_(logger),
          method_server_(MPRPC_REQUIRE_NONULL_MOVE(method_server)) {}

    server(const server&) = delete;
    server& operator=(const server&) = delete;
    server(server&&) = delete;
    server& operator=(server&&) = delete;

    //! destruct
    ~server() { stop(); }

protected:
    //! \copydoc mprpc::impl::server_base::async_process_message
    void async_process_message(
        const std::shared_ptr<transport::session>& session,
        const message_data& data,
        const execution::method_server::on_message_processed_handler& handler)
        override {
        try {
            const auto msg = message(data);
            method_server_->async_process_message(session, msg, handler);
        } catch (const exception& e) {
            MPRPC_ERROR(logger_, "{}", e.info());
            async_remove_session(session);
            return;
        }
    }

private:
    //! logger
    logging::labeled_logger logger_;

    //! method server
    std::shared_ptr<execution::method_server> method_server_;
};

}  // namespace mprpc
