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
 * \brief implementation of the wrapper of mpRPC C++ library for Python
 */
#include <memory>

#include <pybind11/pybind11.h>

#include "mprpc/bind_logging.h"
#include "mprpc/bind_message.h"

PYBIND11_MODULE(_mprpc_cpp, module) {
    module.doc() = "wrapper of mpRPC C++ library for Python";

    mprpc::python::bind_logging(module);
    mprpc::python::bind_message(module);
}
