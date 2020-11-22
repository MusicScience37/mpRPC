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
 * \brief declaration and implementation of function_method_executor class
 */
#pragma once

#include <memory>
#include <type_traits>
#include <utility>

#include "mprpc/execution/method_executor.h"
#include "mprpc/logging/labeled_logger.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/pack_data.h"

namespace mprpc {
namespace execution {

/*!
 * \brief class of executors of methods with functions
 *
 * \tparam FunctionType type of function
 * \tparam FunctionSignature function signature
 */
template <typename FunctionType, typename FunctionSignature>
class function_method_executor;

/*!
 * \brief function_method_executor implementation for functions with non-void
 * result types
 *
 * \tparam FunctionType type of function
 * \tparam ResultType result type
 * \tparam ParamTypes parameter types
 */
template <typename FunctionType, typename ResultType, typename... ParamTypes>
class function_method_executor<FunctionType, ResultType(ParamTypes...)> final
    : public method_executor {
public:
    //! type of function
    using function_type = FunctionType;

    //! type of tuples of parameters
    using params_tuple_type = std::tuple<std::decay_t<ParamTypes>...>;

    /*!
     * \brief construct
     *
     * \param logger logger
     * \param name method name
     * \param function function
     */
    function_method_executor(logging::labeled_logger logger, std::string name,
        function_type function)
        : logger_(std::move(logger)),
          name_(std::move(name)),
          function_(std::move(function)) {}

    //! \copydoc mprpc::execution::method_executor::name
    std::string name() const override { return name_; }

    //! \copydoc mprpc::execution::method_executor::process_request
    message_data process_request(
        const std::shared_ptr<transport::session>& session,
        const message& msg) override {
        try {
            return pack_response(msg.msgid(), invoke(msg));
        } catch (const std::exception& e) {
            MPRPC_ERROR(logger_, "error in method {}: {}", name_, e.what());
            return pack_error_response(msg.msgid(), std::string(e.what()));
        }
    }

    //! \copydoc mprpc::execution::method_executor::process_notification
    void process_notification(
        const std::shared_ptr<transport::session>& session,
        const message& msg) override {
        try {
            invoke(msg);
        } catch (const std::exception& e) {
            MPRPC_ERROR(logger_, "error in method {}: {}", name_, e.what());
        }
    }

private:
    /*!
     * \brief invoke function
     *
     * \param msg message
     * \return result
     */
    ResultType invoke(const message& msg) {
        return invoke_impl(msg.params_as<params_tuple_type>(),
            std::index_sequence_for<ParamTypes...>());
    }

    /*!
     * \brief invoke function
     *
     * \tparam ParamTuple type of parameter tuple
     * \tparam Indices indices
     * \param params parameter tuple
     * \return result
     */
    template <typename ParamTuple, std::size_t... Indices>
    ResultType invoke_impl(
        ParamTuple&& params, std::index_sequence<Indices...> /*unused*/) {
        return function_(std::forward<std::decay_t<ParamTypes>>(
            std::get<Indices>(params))...);
    }

    //! logger
    logging::labeled_logger logger_;

    //! method name
    std::string name_;

    //! function
    function_type function_;
};

/*!
 * \brief function_method_executor implementation for functions with void
 * result types
 *
 * \tparam FunctionType type of function
 * \tparam ParamTypes parameter types
 */
template <typename FunctionType, typename... ParamTypes>
class function_method_executor<FunctionType, void(ParamTypes...)> final
    : public method_executor {
public:
    //! type of function
    using function_type = FunctionType;

    //! type of tuples of parameters
    using params_tuple_type = std::tuple<std::decay_t<ParamTypes>...>;

    /*!
     * \brief construct
     *
     * \param logger logger
     * \param name method name
     * \param function function
     */
    function_method_executor(logging::labeled_logger logger, std::string name,
        function_type function)
        : logger_(std::move(logger)),
          name_(std::move(name)),
          function_(std::move(function)) {}

    //! \copydoc mprpc::execution::method_executor::name
    std::string name() const override { return name_; }

    //! \copydoc mprpc::execution::method_executor::process_request
    message_data process_request(
        const std::shared_ptr<transport::session>& session,
        const message& msg) override {
        try {
            invoke(msg);
            return pack_response(msg.msgid(), msgpack::type::nil_t());
        } catch (const std::exception& e) {
            MPRPC_ERROR(logger_, "error in method {}: {}", name_, e.what());
            return pack_error_response(msg.msgid(), std::string(e.what()));
        }
    }

    //! \copydoc mprpc::execution::method_executor::process_notification
    void process_notification(
        const std::shared_ptr<transport::session>& session,
        const message& msg) override {
        try {
            invoke(msg);
        } catch (const std::exception& e) {
            MPRPC_ERROR(logger_, "error in method {}: {}", name_, e.what());
        }
    }

private:
    /*!
     * \brief invoke function
     *
     * \param msg message
     */
    void invoke(const message& msg) {
        invoke_impl(msg.params_as<params_tuple_type>(),
            std::index_sequence_for<ParamTypes...>());
    }

    /*!
     * \brief invoke function
     *
     * \tparam ParamTuple type of parameter tuple
     * \tparam Indices indices
     * \param params parameter tuple
     */
    template <typename ParamTuple, std::size_t... Indices>
    void invoke_impl(
        ParamTuple&& params, std::index_sequence<Indices...> /*unused*/) {
        function_(std::forward<std::decay_t<ParamTypes>>(
            std::get<Indices>(params))...);
    }

    //! logger
    logging::labeled_logger logger_;

    //! method name
    std::string name_;

    //! function
    function_type function_;
};

/*!
 * \brief create function_method_executor object
 *
 * \tparam FunctionSignature function signature
 * \tparam Function type of function
 * \param logger logger
 * \param name method name
 * \param function function
 * \return function_method_executor object
 */
template <typename FunctionSignature, typename Function>
inline std::shared_ptr<function_method_executor<Function, FunctionSignature>>
make_function_method_executor(
    logging::labeled_logger logger, std::string name, Function&& function) {
    return std::make_shared<
        function_method_executor<Function, FunctionSignature>>(
        std::move(logger), std::move(name), std::forward<Function>(function));
}

}  // namespace execution
}  // namespace mprpc
