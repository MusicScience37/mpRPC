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
 * \brief implementation of load function
 */
#include "mprpc/config/load.h"

#include <stdexcept>

#include <toml.hpp>

#include "mprpc/config/toml_helper.h"
#include "mprpc/error_code.h"
#include "mprpc/exception.h"
#include "mprpc/mprpc_config.h"

namespace mprpc {
namespace config {

mprpc_config load(const std::string& config_filepath) {
    try {
        const auto data = toml::parse(config_filepath);
        return toml::get<mprpc_config>(data);
    } catch (const toml::exception& e) {
        throw exception(error_info(error_code::config_parse_error, e.what()));
    } catch (const std::runtime_error& e) {
        throw exception(error_info(error_code::config_parse_error, e.what()));
    }
}

}  // namespace config
}  // namespace mprpc
