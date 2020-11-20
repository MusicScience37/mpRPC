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
 * \brief declaration of load function
 */
#pragma once

#include <string>

#include "mprpc/mprpc_config.h"
#include "mprpc/mprpc_export.h"

namespace mprpc {
namespace config {

/*!
 * \brief load configuration file
 *
 * \param config_filepath configuration file path
 * \return configuration
 */
MPRPC_EXPORT mprpc_config load(const std::string& config_filepath);

}  // namespace config
}  // namespace mprpc
