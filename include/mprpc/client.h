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
 * \brief declaration and implementation of client class
 */
#pragma once

#include <atomic>
#include <memory>
#include <unordered_map>

#include "mprpc/logging/logger.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/pack_data.h"
#include "mprpc/thread_pool.h"
#include "mprpc/transport/connector.h"
#include "mprpc/typed_response_future.h"

namespace mprpc {

/*!
 * \brief class of clients
 */
class client {
public:
    /*!
     * \brief construct
     *
     * \param logger logger
     * \param threads threads
     * \param connector connector
     */
    client(std::shared_ptr<logging::logger> logger,
        std::shared_ptr<thread_pool> threads,
        std::shared_ptr<transport::connector> connector)
        : logger_(std::move(logger)),
          threads_(std::move(threads)),
          connector_(std::move(connector)) {}

    /*!
     * \brief start process
     */
    void start() {
        MPRPC_INFO(logger_, "client starting");
        threads_->start();
        do_read();
        MPRPC_INFO(logger_, "client started");
    }

    /*!
     * \brief stop process
     */
    void stop() {
        MPRPC_INFO(logger_, "client stopping");
        threads_->stop();
        MPRPC_INFO(logger_, "client stopped");
    }

    /*!
     * \brief asynchronously request
     *
     * \tparam Result result type
     * \tparam Params parameters types
     * \param method method name
     * \param params parameters
     * \return
     */
    template <typename Result, typename... Params>
    typed_response_future<Result> async_request(
        const std::string& method, const Params&... params) {
        response_promise promise(*threads_);
        auto future = promise.get_future();

        while (true) {
            const msgid_t msgid = next_msgid_->fetch_add(1);
            const auto msg =
                pack_request(msgid, method, std::forward_as_tuple(params...));

            std::unique_lock<std::mutex> lock(*mutex_);
            if (requests_.emplace(msgid, request_data{promise}).second) {
                MPRPC_INFO(logger_, "(msgid={}) request {}", msgid, method);
                do_write_request(msgid, msg);
                return typed_response_future<Result>(future);
            }
        }
    }

    client(const client&) = delete;
    client& operator=(const client&) = delete;

    /*!
     * \brief copy
     */
    client(client&&) = default;

    /*!
     * \brief copy
     *
     * \return this object
     */
    client& operator=(client&&) = default;

    //! destruct
    ~client() { stop(); }

private:
    /*!
     * \brief write request
     *
     * \param msgid message ID
     * \param msg message
     */
    void do_write_request(msgid_t msgid, const message_data& msg) {
        connector_->async_write(msg, [this, msgid](const error_info& error) {
            if (error) {
                std::unique_lock<std::mutex> lock(*mutex_);
                requests_.at(msgid).promise.set_error(error);
                requests_.erase(msgid);
            }
        });
    }

    /*!
     * \brief read a message
     */
    void do_read() {
        connector_->async_read(
            [this](const error_info& error, const message_data& data) {
                this->on_read(error, data);
            });
    }

    /*!
     * \brief process on message read
     *
     * \param error error
     * \param data message data
     */
    void on_read(const error_info& error, const message_data& data) {
        if (error) {
            MPRPC_ERROR(logger_, "client stopping on read error");
            std::unique_lock<std::mutex> lock(*mutex_);
            for (auto& req : requests_) {
                req.second.promise.set_error(error);
            }
            return;
        }

        do_read();

        try {
            auto msg = message(data);
            std::unique_lock<std::mutex> lock(*mutex_);
            auto iter = requests_.find(msg.msgid());
            if (iter == requests_.end()) {
                MPRPC_WARN(logger_,
                    "response for message ID {} received, but no request found",
                    msg.msgid());
                return;
            }
            MPRPC_DEBUG(logger_, "(msgid={}) response: {}", msg.msgid(), msg);
            iter->second.promise.set_response(msg);
        } catch (const exception& e) {
            MPRPC_ERROR(logger_, "client stopping on parse error");
            std::unique_lock<std::mutex> lock(*mutex_);
            for (auto& req : requests_) {
                req.second.promise.set_error(error);
            }
            return;
        }
    }

    //! logger
    std::shared_ptr<logging::logger> logger_;

    //! thread pool
    std::shared_ptr<thread_pool> threads_;

    //! connector
    std::shared_ptr<transport::connector> connector_;

    //! next message ID
    std::unique_ptr<std::atomic<msgid_t>> next_msgid_{
        std::make_unique<std::atomic<msgid_t>>(0)};

    //! struct of data for each request
    struct request_data {
        //! promise
        response_promise promise;
    };

    //! data for each request
    std::unordered_map<msgid_t, request_data> requests_{};

    //! mutex
    std::unique_ptr<std::mutex> mutex_{std::make_unique<std::mutex>()};
};

}  // namespace mprpc
