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
 * \brief documentation of namespaces
 */
#pragma once

//! namespace of this project
namespace mprpc {

    //! namespace of internal implementations
    namespace impl {}

    //! namespace of error codes
    namespace error_code {}

    //! namespace of logging functions
    namespace logging {

        //! namespace of internal implementations
        namespace impl {}

    }  // namespace logging

    //! namespace of classes for configurations
    namespace config {}

    //! namespace of classes to transport data
    namespace transport {

        //! namespace of classes of compressors
        namespace compressors {

            //! namespace of internal implementations
            namespace impl {}

        }  // namespace compressors

        //! namespace of classes of parsers
        namespace parsers {

            //! namespace of internal implementations
            namespace impl {}

        }  // namespace parsers

        //! helper classes for asio library
        namespace asio_helper {}

        //! namespace of classes for TCP transport
        namespace tcp {

            //! namespace of internal implementations
            namespace impl {}

        }  // namespace tcp

        //! namespace of classes for UDP transport
        namespace udp {

            //! namespace of internal implementations
            namespace impl {}

        }  // namespace udp

    }  // namespace transport

    //! namespace of classes to execute methods
    namespace execution {}

}  // namespace mprpc
