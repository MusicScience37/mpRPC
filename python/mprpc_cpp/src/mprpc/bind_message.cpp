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
 * \brief implementation of bind_message function
 */
#include "mprpc/bind_message.h"

#include <pybind11/pybind11.h>

#include "mprpc/message_data.h"

namespace mprpc {
namespace python {

void bind_message(pybind11::module& module) {
    using mprpc::message_data;
    pybind11::class_<message_data>(module, "MessageData")
        .def(pybind11::init([](const pybind11::bytes& data) {
            const auto data_as_str = static_cast<std::string>(data);
            return message_data(data_as_str.data(), data_as_str.size());
        }))
        .def("data", [](const message_data& self) {
            if (!self.has_data()) {
                throw pybind11::value_error(
                    "mprpc.message.MessageData.data called to empty data");
            }
            return pybind11::bytes(self.data(), self.size());
        });
}

}  // namespace python
}  // namespace mprpc
