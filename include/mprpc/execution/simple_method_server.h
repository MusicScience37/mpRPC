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
 * \brief declaration and implementation of simple_method_server class
 */
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "mprpc/execution/method_executor.h"
#include "mprpc/execution/method_server.h"
#include "mprpc/logging/logger.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/thread_pool.h"

namespace mprpc {
namespace execution {

/*!
 * \brief class of simple servers to execute methods
 */
class simple_method_server : public method_server {
public:
    /*!
     * \brief construct
     *
     * \param logger logger
     * \param threads threads
     * \param methods methods
     */
    simple_method_server(std::shared_ptr<mprpc::logging::logger> logger,
        std::shared_ptr<mprpc::thread_pool> threads,
        const std::vector<std::shared_ptr<method_executor>>& methods)
        : logger_(std::move(logger)), threads_(std::move(threads)) {
        for (const auto& method : methods) {
            methods_.emplace(method->name(), method);
        }
    }

    //! \copydoc mprpc::execution::method_server::async_process_message
    void async_process_message(
        const std::shared_ptr<transport::session>& session, const message& msg,
        on_message_processed_handler handler) override {
        threads_->post(
            [this, session, msg, moved_handler = std::move(handler)] {
                this->do_process_message(session, msg, moved_handler);
            });
    }

private:
    /*!
     * \brief process message
     *
     * \param session session
     * \param msg message
     * \param handler handler
     */
    void do_process_message(const std::shared_ptr<transport::session>& session,
        const message& msg, const on_message_processed_handler& handler) {
        switch (msg.type()) {
        case msgtype::request: {
            MPRPC_INFO(logger_, "{} request {}",
                session->remote_address()->full_address(), msg.method());
            MPRPC_DEBUG(logger_, "params: {}", msg.params());
            const auto method = find_method(msg.method(), handler);
            if (!method) {
                return;
            }
            const auto response = method->process_request(session, msg);
            MPRPC_DEBUG(logger_, "response: {}", response);
            handler(error_info(), true, response);
            break;
        }
        case msgtype::notification: {
            MPRPC_INFO(logger_, "{} notify {}",
                session->remote_address()->full_address(), msg.method());
            MPRPC_DEBUG(logger_, "params: {}", msg.params());
            const auto method = find_method(msg.method(), handler);
            if (!method) {
                return;
            }
            method->process_notification(session, msg);
            MPRPC_DEBUG(logger_, "executed method {}", msg.method());
            handler(error_info(), false, message_data());
            break;
        }
        default:
            MPRPC_ERROR(logger_, "invalid message type {}",
                static_cast<int>(msg.type()));
            handler(
                error_info(error_code::invalid_message, "invalid message type"),
                false, message_data());
        }
    }

    /*!
     * \brief find method
     *
     * \param name method name
     * \param handler handler
     * \return method (if exists)
     */
    std::shared_ptr<method_executor> find_method(
        const std::string& name, const on_message_processed_handler& handler) {
        auto iter = methods_.find(name);
        if (iter == methods_.end()) {
            MPRPC_ERROR(logger_, "method {} not found", name);
            handler(error_info(error_code::method_not_found,
                        "method " + name + " not found"),
                false, message_data());
            return nullptr;
        }
        return iter->second;
    }

    //! logger
    std::shared_ptr<mprpc::logging::logger> logger_;

    //! thread pool
    std::shared_ptr<mprpc::thread_pool> threads_;

    //! map of methods
    std::unordered_map<std::string, std::shared_ptr<method_executor>>
        methods_{};
};

}  // namespace execution
}  // namespace mprpc
