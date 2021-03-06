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
 * \brief declaration of bind_config function
 */
#pragma once

#include <pybind11/pybind11.h>

namespace mprpc {
namespace python {

/*!
 * \brief bind configuration-related functions, classes, and enumerations
 *
 * \param module module
 */
void bind_config(pybind11::module& module);

}  // namespace python
}  // namespace mprpc
