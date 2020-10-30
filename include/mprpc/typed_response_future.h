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
 * \brief declaration and implementation of typed_response_future class
 */
#pragma once

#include <memory>

#include "mprpc/message.h"
#include "mprpc/pack_data.h"
#include "mprpc/response_future.h"

namespace mprpc {

/*!
 * \brief class of futures of responses with convertion to result types
 *
 * \tparam ResultType result type
 */
template <typename ResultType>
class typed_response_future {
public:
    //! result type
    using result_type = ResultType;

    /*!
     * \brief construct (empty)
     */
    typed_response_future() = default;

    /*!
     * \brief construct
     *
     * \param future response future
     */
    explicit typed_response_future(response_future future)
        : future_(std::move(future)) {}

    /*!
     * \brief set handler
     *
     * \note ResultType must be default constructible
     *
     * \param handler handler
     */
    void then(
        const stl_ext::shared_function<void(const error_info&, result_type)>&
            handler) {
        then(
            [handler](result_type result) {
                handler(error_info(), std::move(result));
            },
            [handler](
                const error_info& error) { handler(error, result_type()); });
    }

    /*!
     * \brief set handlers
     *
     * \param on_success handler on success
     * \param on_failure handler on failure
     */
    void then(const stl_ext::shared_function<void(result_type)>& on_success,
        const stl_ext::shared_function<void(const error_info&)>& on_failure) {
        future_.then(
            [on_success, on_failure](const message_data& msg) {
                try {
                    auto parsed = message(msg);
                    if (parsed.has_error()) {
                        on_failure(error_info(error_code::unexpected_error,
                            "error on server", pack_data(parsed.error())));
                        return;
                    }
                    on_success(parsed.result_as<result_type>());
                } catch (const exception& e) {
                    on_failure(e.info());
                }
            },
            [on_failure](const error_info& error) { on_failure(error); });
    }

    /*!
     * \brief get std::future object for response
     *
     * \return std::future object
     */
    std::future<result_type> get_future() {
        auto promise = std::make_shared<std::promise<result_type>>();
        auto future = promise->get_future();
        then([promise](
                 result_type result) { promise->set_value(std::move(result)); },
            [promise](const error_info& error) {
                promise->set_exception(
                    std::make_exception_ptr(exception(error)));
            });
        return future;
    }

private:
    //! response future
    response_future future_{};
};

}  // namespace mprpc
