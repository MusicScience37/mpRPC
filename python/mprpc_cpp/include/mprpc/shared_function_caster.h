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
 * \brief declaration and implementation of type_caster<shared_function> class
 */
#pragma once

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <stl_ext/shared_function.h>

namespace pybind11 {
namespace detail {

/*!
 * \brief implementation of pybind11::detail::type_caster for
 *        stl_ext::shared_function class
 *
 * \tparam Result result type
 * \tparam Args arguments types
 */
template <typename Result, typename... Args>
class type_caster<stl_ext::shared_function<Result(Args...)>> {
public:
    // NOLINTNEXTLINE: external library
    PYBIND11_TYPE_CASTER(
        stl_ext::shared_function<Result(Args...)>, _("shared_function"));

    /*!
     * \brief convert a Python object to a C++ object
     *
     * \param src Python object
     * \param use_implicit_cast use implicit conversions
     * \return whether convertion succeeded
     */
    bool load(handle src, bool use_implicit_cast) {
        bool result = function_caster_.load(src, use_implicit_cast);
        value = static_cast<std::function<Result(Args...)>>(function_caster_);
        return result;
    }

    /*!
     * \brief convert a C++ object to a Python object
     *
     * \param src C++ object
     * \param policy return value policy
     * \param parent parent object
     * \return Python object
     */
    static handle cast(const stl_ext::shared_function<Result(Args...)>& src,
        return_value_policy policy, handle parent) {
        return type_caster<std::function<Result(Args...)>>::cast(
            src, policy, parent);
    }

private:
    //! type_caster of functions
    type_caster<std::function<Result(Args...)>> function_caster_;
};

}  // namespace detail
}  // namespace pybind11
