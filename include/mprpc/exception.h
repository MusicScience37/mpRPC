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
 * \brief declaration and implementation of exception class
 */
#pragma once

#include <exception>

#include "mprpc/error_info.h"

namespace mprpc {

/*!
 * \brief class of exception in this project
 */
class exception final : public std::exception {
public:
    /*!
     * \brief construct
     *
     * \param info error information
     */
    explicit exception(error_info info) noexcept : info_(std::move(info)) {}

    /*!
     * \brief get error message
     *
     * \return error message
     */
    const char* what() const noexcept override {
        return info_.message().c_str();
    }

    /*!
     * \brief get error information
     *
     * \return error information
     */
    const error_info& info() const noexcept { return info_; }

private:
    //! error information
    error_info info_;
};

}  // namespace mprpc
