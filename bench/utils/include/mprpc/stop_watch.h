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
 * \brief declaration and implementation of stop_watch class
 */
#pragma once

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <vector>

#include <fmt/core.h>

namespace mprpc {

/*!
 * \brief class of stop watch
 */
class stop_watch {
public:
    //! construct
    stop_watch() = default;

    /*!
     * \brief reserve memory
     *
     * \param size size of vectors
     */
    void reserve(std::size_t size) { durations_nano_.reserve(size); }

    /*!
     * \brief start
     */
    void start() { begin_time_ = cloc_type::now(); }

    /*!
     * \brief stop
     */
    void stop() {
        const auto duration = cloc_type::now() - begin_time_;
        const auto nanoseconds =
            std::chrono::duration_cast<std::chrono::nanoseconds>(duration)
                .count();
        durations_nano_.push_back(static_cast<std::uint64_t>(nanoseconds));
    }

    /*!
     * \brief write result to file
     *
     * \param filepath filepath
     */
    void write(const std::string& filepath) {
        auto* file = std::fopen(filepath.c_str(), "w");
        const auto size = durations_nano_.size();
        for (std::size_t i = 0; i < size; ++i) {
            fmt::print(file, "{}\n", durations_nano_[i]);
        }
        std::fclose(file);
    }

private:
    //! cloc type
    using cloc_type = std::chrono::steady_clock;

    //! time point type
    using time_point_type = cloc_type::time_point;

    //! beggining time
    time_point_type begin_time_{};

    //! vector of durations in nanoseconds
    std::vector<std::uint64_t> durations_nano_{};
};

}  // namespace mprpc
