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
 * \brief declaration and implementation of shared_function class
 */
#pragma once

#include <stdexcept>

#include "stl_ext/shared_any.h"

namespace stl_ext {

namespace impl {

/*!
 * \brief internal implementation for shared_function class
 *
 * \tparam FunctionSignature function signature
 * \tparam Function function type
 */
template <typename FunctionSignature, typename Function>
class shared_function_impl;

/*!
 * \brief shared_function_impl implementation
 *
 * \tparam Result result type
 * \tparam Args arguments types
 * \tparam Function function type
 */
template <typename Result, typename... Args, typename Function>
class shared_function_impl<Result(Args...), Function> {
public:
    /*!
     * \brief invoke function
     *
     * \param function function
     * \param args arguments
     * \return result
     */
    static Result invoke(const shared_any& function, Args... args) {
        auto* ptr = function.void_ptr();
        return (*static_cast<Function*>(ptr))(std::forward<Args>(args)...);
    }
};

}  // namespace impl

/*!
 * \brief exception of incorrect shared_function call
 */
class bad_shared_function_call : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

/*!
 * \brief class of function pointer shared with reference count
 *
 * \tparam FunctionSignature function signature
 */
template <typename FunctionSignature>
class shared_function;

/*!
 * \brief shared_function implementation
 *
 * \tparam Result result type
 * \tparam Args arguments types
 */
template <typename Result, typename... Args>
class shared_function<Result(Args...)> {
public:
    //! construct null object
    shared_function() noexcept = default;

    //! construct null object
    shared_function(std::nullptr_t) noexcept {}  // NOLINT

    /*!
     * \brief construct with function object
     *
     * \tparam Function function type
     * \param function function
     */
    template <typename Function,
        typename = std::enable_if_t<
            !std::is_same<std::decay_t<Function>, shared_function>::value>>
    shared_function(Function&& function)  // NOLINT: use implicit convertion
        : func_(std::forward<Function>(function)),
          invoker_(&impl::shared_function_impl<Result(Args...),
                   std::decay_t<Function>>::invoke) {}

    /*!
     * \brief assign null function object
     *
     * \return this object
     */
    shared_function& operator=(std::nullptr_t) noexcept {
        func_.reset();
        return *this;
    }

    /*!
     * \brief assign function object
     *
     * \tparam Function function type
     * \param function function
     * \return this object
     */
    template <typename Function,
        typename = std::enable_if_t<
            !std::is_same<std::decay_t<Function>, shared_function>::value>>
    shared_function& operator=(Function&& function) {
        func_ = std::forward<Function>(function);
        invoker_ = &impl::shared_function_impl<Result(Args...),
            std::decay_t<Function>>::invoke;
        return *this;
    }

    /*!
     * \brief invoke this function
     *
     * \param args arguments
     * \return result
     */
    Result operator()(Args... args) const {
        if (!func_.has_value()) {
            throw bad_shared_function_call(
                "shared_function::operator() called for empty object");
        }
        return invoker_(func_, std::forward<Args>(args)...);
    }

    /*!
     * \brief get whether this object has a function object
     *
     * \return whether this object has a function object
     */
    explicit operator bool() const noexcept { return func_.has_value(); }

private:
    //! type of invoker
    using invoker_type = Result (*)(const shared_any&, Args...);

    //! function
    shared_any func_{};

    //! invoker
    invoker_type invoker_{nullptr};
};

}  // namespace stl_ext
