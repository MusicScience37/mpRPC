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
 * \brief declaration and implementation of log_data_traits class
 */
#pragma once

namespace mprpc {
namespace logging {

/*!
 * \brief traits class for data used in logs
 *
 * \note This class is default implementation.
 * Specialize this class to modify behavior for specific data types.
 *
 * \tparam DataType data type
 */
template <typename DataType>
struct log_data_traits {
    //! data type
    using data_type = DataType;

    /*!
     * \brief preprocess data
     *
     * \param data data
     * \return data (unmodified in this default implementation)
     */
    static const data_type& preprocess(const data_type& data) noexcept {
        return data;
    }
};

}  // namespace logging
}  // namespace mprpc
