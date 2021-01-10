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
 * \brief implementation of bind_transport function
 */
#include "mprpc/bind_transport.h"

#include <fmt/core.h>

#include "mprpc/transport/address.h"
#include "mprpc/transport/session.h"

namespace mprpc {
namespace python {

void bind_transport(pybind11::module& module) {
    // module name in Python
    static const std::string module_name = "mprpc.transport";

    std::string class_full_name = module_name + ".Address";
    using mprpc::transport::address;
    pybind11::class_<address, std::shared_ptr<address>>(module, "Address",
        R"doc(Address()

            base class of addresses

            Attributes
            ----------
            full_address : str
                full address

            Methods
            -------
            __str__(self) -> str
                get string representation for debugging
        )doc")
        .def_property_readonly("full_address", &address::full_address)
        .def("__str__",
            [class_full_name](const std::shared_ptr<address>& self) {
                return fmt::format("<{}: full_address={}>", class_full_name,
                    self->full_address());
            })
        .def("__repr__",
            [class_full_name](const std::shared_ptr<address>& self) {
                return fmt::format("<{}: full_address={}>", class_full_name,
                    self->full_address());
            });

    class_full_name = module_name + ".Session";
    using mprpc::transport::session;
    pybind11::class_<session, std::shared_ptr<session>>(module, "Session",
        R"doc(Session()

            base class of sessions

            Attributes
            ----------
            remote_address : mprpc.transport.Address
                address of remote endpoint
        )doc")
        .def("shutdown", &session::shutdown, R"doc(shutdown()

            shutdown connection gracefully
        )doc")
        .def_property_readonly("remote_address", &session::remote_address)
        .def("__str__",
            [class_full_name](const std::shared_ptr<session>& self) {
                return fmt::format("<{}: remote_address={}>", class_full_name,
                    self->remote_address()->full_address());
            })
        .def("__repr__",
            [class_full_name](const std::shared_ptr<session>& self) {
                return fmt::format("<{}: remote_address={}>", class_full_name,
                    self->remote_address()->full_address());
            });
}

}  // namespace python
}  // namespace mprpc
