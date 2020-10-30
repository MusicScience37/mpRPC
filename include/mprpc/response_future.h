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
 * \brief declaration and implementation of response_future class
 */
#pragma once

#include <future>
#include <memory>
#include <mutex>

#include <stl_ext/shared_function.h>

#include "mprpc/error_info.h"
#include "mprpc/exception.h"
#include "mprpc/message_data.h"
#include "mprpc/thread_pool.h"

namespace mprpc {
namespace impl {

/*!
 * \brief class of data in response_future
 */
class response_future_data {
public:
    //! type of handlers on responses received
    using on_response_handler_type =
        stl_ext::shared_function<void(const error_info&, const message_data&)>;

    /*!
     * \brief construct
     *
     * \param threads thread pool
     */
    explicit response_future_data(thread_pool& threads) : threads_(threads) {}

    /*!
     * \brief set error
     *
     * \param error error
     */
    void set_error(const error_info& error) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (has_response_) {
            throw exception(error_info(error_code::invalid_future_use,
                "this future already has a response or an error"));
        }
        has_response_ = true;

        if (!on_response_) {
            error_ = error;
            return;
        }

        threads_.post([error, on_response = on_response_] {
            on_response(error, message_data());
        });
    }

    /*!
     * \brief set response message
     *
     * \param msg response message
     */
    void set_response(const message_data& msg) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (has_response_) {
            throw exception(error_info(error_code::invalid_future_use,
                "this future already has a response or an error"));
        }
        has_response_ = true;

        if (!on_response_) {
            msg_ = msg;
            return;
        }

        threads_.post([msg, on_response = on_response_] {
            on_response(error_info(), msg);
        });
    }

    /*!
     * \brief set handler on responses received
     *
     * \param handler handler
     */
    void set_handler(const on_response_handler_type& handler) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (has_handler_) {
            throw exception(error_info(error_code::invalid_future_use,
                "this future already has a handler"));
        }
        has_handler_ = true;

        if (!has_response_) {
            on_response_ = handler;
            return;
        }

        threads_.post(
            [handler, msg = msg_, error = error_] { handler(error, msg); });
    }

private:
    //! threads
    thread_pool& threads_;

    //! whether this object has a response or an error
    bool has_response_{false};

    //! error
    error_info error_{};

    //! response message
    message_data msg_{};

    //! whether this object has a handler on responses
    bool has_handler_{false};

    //! handler on responses received
    on_response_handler_type on_response_{};

    //! mutex
    std::mutex mutex_{};
};

}  // namespace impl

/*!
 * \brief class of futures for response messages
 */
class response_future {
public:
    /*!
     * \brief construct (empty)
     */
    response_future() = default;

    /*!
     * \brief construct
     *
     * \param data data
     */
    explicit response_future(std::shared_ptr<impl::response_future_data> data)
        : data_(std::move(data)) {}

    /*!
     * \brief set handler
     *
     * \param handler handler
     */
    void then(const stl_ext::shared_function<void(
            const error_info&, const message_data&)>& handler) {
        require_data();
        data_->set_handler(handler);
    }

    /*!
     * \brief set handlers
     *
     * \param on_success handler on success
     * \param on_failure handler on failure
     */
    void then(
        const stl_ext::shared_function<void(const message_data&)>& on_success,
        const stl_ext::shared_function<void(const error_info&)>& on_failure) {
        require_data();
        data_->set_handler([on_success, on_failure](const error_info& error,
                               const message_data& msg) {
            if (error) {
                on_failure(error);
            } else {
                on_success(msg);
            }
        });
    }

    /*!
     * \brief get std::future object for response
     *
     * \return std::future object
     */
    std::future<message_data> get_future() {
        std::promise<message_data> promise;
        auto future = promise.get_future();
        data_->set_handler(
            [moved_promise = std::move(promise)](const mprpc::error_info& error,
                const mprpc::message_data& msg) mutable {
                if (error) {
                    moved_promise.set_exception(
                        std::make_exception_ptr(exception(error)));
                } else {
                    moved_promise.set_value(msg);
                }
            });
        return future;
    }

private:
    /*!
     * \brief check that data is not empty
     */
    void require_data() const {
        if (!data_) {
            throw exception(error_info(
                error_code::invalid_future_use, "tried to use empty future"));
        }
    }

    //! data
    std::shared_ptr<impl::response_future_data> data_{};
};

}  // namespace mprpc
