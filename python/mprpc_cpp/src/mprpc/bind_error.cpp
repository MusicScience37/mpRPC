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
 * \brief implementation of bind_error function
 */
#include "mprpc/bind_error.h"

#include "mprpc/error_info.h"
#include "mprpc/exception.h"
#include "mprpc/repr_from_cpp.h"

namespace mprpc {
namespace python {

void bind_error(pybind11::module& module) {
    // module name in Python
    static const std::string module_name = "mprpc";

    using mprpc::error_code_t;
    using mprpc::error_info;
    using mprpc::message_data;
    std::string class_full_name = module_name + ".ErrorInfo";
    pybind11::class_<error_info>(module, "ErrorInfo",
        R"doc(ErrorInfo(code: int = 0, message: str = '', data: MessageData = MessageData())

            class of error information

            Parameters
            ----------
            code : int, optional
                error code.
                Zero for this parameter means no error, so other arguments are ignores.
            message : str, optional
                error message
            data : mprpc.message.MessageData, optional
                data related to this error

            Attributes
            ----------
            code : int
                error code (read only)
            message : str
                error message (read only)
            data : mprpc.message.MessageData
                data related to this error (read only)

            Methods
            -------
            __bool__(self) -> bool
                get whether this has an error
            __str__(self) -> str
                get string representation for debugging
            __repr__(self) -> str
                get string representation usable with eval()
        )doc")
        .def(pybind11::init([](error_code_t code, const std::string& message,
                                const message_data& data) {
            if (code == mprpc::error_code::success) {
                return error_info();
            }
            if (!data.has_data()) {
                return error_info(code, message);
            }
            return error_info(code, message, data);
        }),
            pybind11::arg("code") =
                static_cast<mprpc::error_code_t>(mprpc::error_code::success),
            pybind11::arg("message") = std::string(),
            pybind11::arg("data") = message_data())
        .def("has_error", &error_info::has_error,
            R"doc(has_error(self) -> bool

            get whether this has an error

            Returns
            -------
            bool
              whether this has an error
            )doc")
        .def("__bool__", &error_info::has_error)
        .def("has_data", &error_info::has_data,
            R"doc(has_data(self) -> bool

            get whether this has data related to this error

            Parameters
            ----------

            Returns
            -------
            bool
              whether this has data related to this error
            )doc")
        .def_property_readonly("code", &error_info::code)
        .def_property_readonly("message", &error_info::message)
        .def_property_readonly("data", &error_info::data)
        .def("__str__",
            [class_full_name](const error_info& self) {
                if (!self.has_error()) {
                    return fmt::format("{}()", class_full_name);
                }
                if (!self.has_data()) {
                    return fmt::format("{}(code={}, message={})",
                        class_full_name, self.code(),
                        repr_from_cpp(self.message()));
                }
                return fmt::format("{}(code={}, message={}, data={})",
                    class_full_name, self.code(), repr_from_cpp(self.message()),
                    str_from_cpp(self.data()));
            })
        .def("__repr__", [class_full_name](const error_info& self) {
            if (!self.has_error()) {
                return fmt::format("{}()", class_full_name);
            }
            if (!self.has_data()) {
                return fmt::format("{}(code={}, message={})", class_full_name,
                    self.code(), repr_from_cpp(self.message()));
            }
            return fmt::format("{}(code={}, message={}, data={})",
                class_full_name, self.code(), repr_from_cpp(self.message()),
                repr_from_cpp(self.data()));
        });

    static pybind11::exception<mprpc::exception> mprpc_exception(
        module, "MPRPCException");
    // NOLINTNEXTLINE: requirements of external library
    pybind11::register_exception_translator([](std::exception_ptr ptr) {
        try {
            if (ptr) {
                std::rethrow_exception(ptr);
            }
        } catch (const mprpc::exception& except) {
            PyErr_SetObject(
                mprpc_exception.ptr(), pybind11::cast(except.info()).ptr());
        }
    });
}

}  // namespace python
}  // namespace mprpc
