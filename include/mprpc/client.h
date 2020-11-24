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

#include "mprpc/client_config.h"
#include "mprpc/logging/labeled_logger.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/pack_data.h"
#include "mprpc/require_nonull.h"
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
     * \param config configuration
     */
    client(logging::labeled_logger logger, std::shared_ptr<thread_pool> threads,
        std::shared_ptr<transport::connector> connector, client_config config)
        : logger_(std::move(logger)),
          threads_(MPRPC_REQUIRE_NONULL_MOVE(threads)),
          connector_(MPRPC_REQUIRE_NONULL_MOVE(connector)),
          config_(std::move(config)) {}

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
        connector_->shutdown();
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
     * \return future
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

    /*!
     * \brief synchronously request
     *
     * \tparam Result result type
     * \tparam Params parameters types
     * \param method method name
     * \param params parameters
     * \return result
     */
    template <typename Result, typename... Params>
    Result request(const std::string& method, const Params&... params) {
        auto future = async_request<Result>(method, params...).get_future();
        if (future.wait_for(std::chrono::milliseconds(
                config_.sync_request_timeout_ms.value())) !=
            std::future_status::ready) {
            MPRPC_ERROR(
                logger_, "timeout in synchronous request for {}", method);
            throw exception(error_info(error_code::timeout,
                "timeout in synchronous request for " + method));
        }
        return future.get();
    }

    /*!
     * \brief send notification
     *
     * \tparam Params parameters types
     * \param method method name
     * \param params parameters
     */
    template <typename... Params>
    void notify(const std::string& method, const Params&... params) {
        const auto msg =
            pack_notification(method, std::forward_as_tuple(params...));
        connector_->async_write(msg, [this, method](const error_info& error) {
            if (error) {
                MPRPC_WARN(
                    logger_, "failed to send notification of {}", method);
            }
        });
    }

    client(const client&) = delete;
    client& operator=(const client&) = delete;
    client(client&&) = delete;
    client& operator=(client&&) = delete;

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
            if (msg.type() != msgtype::response) {
                throw exception(error_info(error_code::invalid_message,
                    "message type is not response"));
            }

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
            MPRPC_ERROR(logger_, "client stopping on error: {}", e.info());
            std::unique_lock<std::mutex> lock(*mutex_);
            for (auto& req : requests_) {
                req.second.promise.set_error(error);
            }
            return;
        }
    }

    //! logger
    logging::labeled_logger logger_;

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

    //! configuration
    const client_config config_;
};

}  // namespace mprpc
