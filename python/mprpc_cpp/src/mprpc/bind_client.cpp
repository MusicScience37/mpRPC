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
 * \brief implementation of bind_client function
 */
#include "mprpc/bind_client.h"

#include "mprpc/python_client_helper.h"

namespace mprpc {
namespace python {

void bind_client(pybind11::module& module) {
    using mprpc::python::python_client_helper;
    pybind11::class_<python_client_helper,
        std::shared_ptr<python_client_helper>>(module, "PythonClientHelper",
        R"doc(PythonClientHelper(logger: mprpc.logging.Logger, config: mprpc.config.ClientConfig, message_processor: Callable[[mprpc.ErrorInfo, mprpc.message.MessageData], Any])

            class for implementation of clients in Python

            Parameters
            ----------
            logger : mprpc.logging.Logger
                logger
            config : mprpc.config.ClientConfig
                client configuration
            message_processor : Callable[[mprpc.ErrorInfo, mprpc.message.MessageData], Any]
                function to process messages.
                Parameters are error and message data.
            )doc")
        .def("start", &python_client_helper::start, R"doc(start()

            start process
            )doc")
        .def("stop", &python_client_helper::stop, R"doc(stop()

            stop process
            )doc")
        .def("async_send", &python_client_helper::async_send,
            R"doc(async_send(data: mprpc.message.MessageData, handler: Callable[[mprpc.ErrorInfo], Any])

            asynchronously send a request

            Parameters
            ----------
            data : mprpc.message.MessageData
                message data
            handler : Callable[[mprpc.ErrorInfo], Any]
                handler on message sended
                Parameter is error.
            )doc");
}

}  // namespace python
}  // namespace mprpc
