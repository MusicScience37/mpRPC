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
 * \brief declaration and implementation of repr_from_cpp function
 */
#pragma once

#include <pybind11/pybind11.h>

namespace mprpc {
namespace python {

/*!
 * \brief call Python's repr function for C++ object
 *
 * \tparam T object type
 * \param obj object
 * \return result string
 */
template <typename T>
inline std::string repr_from_cpp(const T& obj) {
    return static_cast<std::string>(pybind11::repr(pybind11::cast(obj)));
}

/*!
 * \brief call Python's str function for C++ object
 *
 * \tparam T object type
 * \param obj object
 * \return result string
 */
template <typename T>
inline std::string str_from_cpp(const T& obj) {
    return static_cast<std::string>(pybind11::str(pybind11::cast(obj)));
}

}  // namespace python
}  // namespace mprpc
