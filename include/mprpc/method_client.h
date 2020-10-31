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
 * \brief declaration and implementation of method_client class
 */
#pragma once

#include "mprpc/client.h"

namespace mprpc {

/*!
 * \brief class of clients for methods
 *
 * \tparam FunctionSignature function signature of methods
 */
template <typename FunctionSignature>
class method_client;

/*!
 * \brief implementation of method_client
 *
 * \tparam ResultType
 * \tparam ParamsType
 */
template <typename ResultType, typename... ParamsType>
class method_client<ResultType(ParamsType...)> {
public:
    /*!
     * \brief construct
     *
     * \param client client
     * \param method method name
     */
    method_client(client& client, std::string method)
        : client_(client), method_(std::move(method)) {}

    /*!
     * \brief asynchronously request
     *
     * \param params parameters
     * \return future
     */
    typed_response_future<ResultType> async_request(
        const ParamsType&... params) {
        return client_.async_request<ResultType>(method_, params...);
    }

    /*!
     * \brief synchronously request
     *
     * \param params parameters
     * \return result
     */
    ResultType request(const ParamsType&... params) {
        return client_.request<ResultType>(method_, params...);
    }

    /*!
     * \brief synchronously request
     *
     * \param params parameters
     * \return result
     */
    ResultType operator()(const ParamsType&... params) {
        return request(params...);
    }

    /*!
     * \brief send notification
     *
     * \param params parameters
     */
    void notify(const ParamsType&... params) {
        client_.notify(method_, params...);
    }

private:
    //! client
    client& client_;

    //! method name
    std::string method_;
};

}  // namespace mprpc
