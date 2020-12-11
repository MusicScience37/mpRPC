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
 * \brief implementation of the wrapper of C++ classes for tests
 */
#include <pybind11/pybind11.h>

#include "mock_session.h"
#include "mprpc/shared_function_caster.h"
#include "mprpc/transport/mock/mock_address.h"

PYBIND11_MODULE(mprpc_cpp_test, module) {
    module.doc() = "wrapper of C++ classes for tests of mpRPC";

    pybind11::module_::import("_mprpc_cpp");

    using mprpc::transport::address;
    using mprpc::transport::mock::mock_address;
    pybind11::class_<mock_address, address, std::shared_ptr<mock_address>>(
        module, "MockAddress")
        .def(pybind11::init<>());

    using mprpc::transport::session;
    using mprpc::transport::mock::python_mock_session;
    pybind11::class_<python_mock_session, session,
        std::shared_ptr<python_mock_session>>(module, "MockSession")
        .def(pybind11::init<>());

    module.def("call_int_adder",
        [](const stl_ext::shared_function<int(int, int)>& callback, int x,
            int y) { return callback(x, y); });
}
