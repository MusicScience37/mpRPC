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
 * \brief implementation of bind_server function
 */
#include "mprpc/bind_server.h"

#include "mprpc/python_server_helper.h"

namespace mprpc {
namespace python {

void bind_server(pybind11::module& module) {
    using mprpc::python::python_server_helper;
    pybind11::class_<python_server_helper,
        std::shared_ptr<python_server_helper>>(module, "PythonServerHelper",
        R"doc(PythonServerHelper(logger: mprpc.logging.Logger, config: mprpc.config.ServerConfig, message_processor: Callable[[mprpc.transport.Session, mprpc.message.MessageData], mprpc.message.MessageData])

            class for implementation of servers in Python

            Parameters
            ----------
            logger : mprpc.logging.Logger
                logger
            config : mprpc.config.ServerConfig
                server configuration
            message_processor: Callable[[mprpc.transport.Session, mprpc.message.MessageData], mprpc.message.MessageData]
                function to process messages
        )doc")
        .def(pybind11::init(&python_server_helper::create))
        .def("start", &python_server_helper::start, R"doc(start()

            start process
        )doc")
        .def("stop", &python_server_helper::stop, R"doc(stop()

            stop process
        )doc");
}

}  // namespace python
}  // namespace mprpc
