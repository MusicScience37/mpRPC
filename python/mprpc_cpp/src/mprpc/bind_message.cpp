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

#include <fmt/format.h>
#include <pybind11/pybind11.h>

#include "mprpc/format_message.h"
#include "mprpc/message_data.h"

namespace mprpc {
namespace python {

pybind11::bytes message_data_to_bytes(const message_data& self) {
    if (!self.has_data()) {
        throw pybind11::value_error(
            "mprpc.message.MessageData.data called to empty data");
    }
    return pybind11::bytes(self.data(), self.size());
}

void bind_message(pybind11::module& module) {
    using mprpc::message_data;

    // module name in Python
    static const std::string module_name = "mprpc.message";

    std::string class_full_name = module_name + ".MessageData";
    pybind11::class_<message_data>(
        module, "MessageData", "class of message data")
        .def(pybind11::init<>())
        .def(pybind11::init([](const pybind11::bytes& data) {
            const auto data_as_str = static_cast<std::string>(data);
            return message_data(data_as_str.data(), data_as_str.size());
        }))
        .def("data", &message_data_to_bytes,
            R"doc(
            get data

            Parameters
            ----------

            Returns
            -------
            bytes
              data
            )doc")
        .def("__bytes__", &message_data_to_bytes)
        .def("__str__",
            [class_full_name](const message_data& self) {
                if (!self.has_data()) {
                    return class_full_name + "()";
                }
                return fmt::format(
                    "<{}: {}>", class_full_name, mprpc::format_message(self));
            })
        .def("__repr__",
            [class_full_name](const message_data& self) {
                if (!self.has_data()) {
                    return class_full_name + "()";
                }
                return fmt::format("{}({})", class_full_name,
                    static_cast<std::string>(pybind11::repr(
                        pybind11::bytes(self.data(), self.size()))));
            })
        .def("__eq__",
            [](const message_data& self, const message_data& other) {
                return self == other;
            })
        .def("__ne__", [](const message_data& self, const message_data& other) {
            return self != other;
        });
}

}  // namespace python
}  // namespace mprpc
