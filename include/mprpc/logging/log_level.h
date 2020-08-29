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
 * \brief definition of log_level enumeration
 */
#pragma once

namespace mprpc {
namespace logging {

/*!
 * \brief log level enumeration
 */
enum class log_level {
    trace,  //!< trace
    debug,  //!< debug
    info,   //!< information
    warn,   //!< warning
    error,  //!< error
    fatal,  //!< fatal
    none    //!< none (for log output level)
};

}  // namespace logging
}  // namespace mprpc
