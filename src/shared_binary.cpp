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
 * \brief implementation of shared_binary class
 */
#include "mprpc/shared_binary.h"

#include <algorithm>
#include <atomic>
#include <memory>
#include <utility>
#include <vector>

#include "mprpc/require_nonull.h"

namespace mprpc {

//! struct for internal data
struct shared_binary::impl {
    //! data
    std::vector<char> data{};

    //! reference count
    std::atomic<std::size_t> ref{1};

    /*!
     * \brief construct
     *
     * \param data_in data
     */
    explicit impl(std::vector<char> data_in) noexcept
        : data(std::move(data_in)) {}
};

shared_binary::shared_binary() noexcept = default;

shared_binary::shared_binary(const char* data, std::size_t size) {
    MPRPC_REQUIRE_NONULL(data);
    auto temp_impl =
        std::make_unique<impl>(std::vector<char>(data, data + size));
    impl_ = temp_impl.release();
}

shared_binary::shared_binary(const shared_binary& data) noexcept
    : impl_(data.impl_) {
    if (impl_ != nullptr) {
        ++(impl_->ref);
    }
}

shared_binary::shared_binary(shared_binary&& data) noexcept
    : impl_(std::exchange(data.impl_, nullptr)) {}

shared_binary& shared_binary::operator=(const shared_binary& data) noexcept {
    if (this == &data) {
        return *this;
    }
    reset();
    impl_ = data.impl_;
    if (impl_ != nullptr) {
        ++(impl_->ref);
    }
    return *this;
}

shared_binary& shared_binary::operator=(shared_binary&& data) noexcept {
    reset();
    impl_ = std::exchange(data.impl_, nullptr);
    return *this;
}

shared_binary::~shared_binary() { reset(); }

bool shared_binary::has_data() const noexcept { return impl_ != nullptr; }

const char* shared_binary::data() const noexcept {
    if (impl_ == nullptr) {
        return nullptr;
    }
    return impl_->data.data();
}

std::size_t shared_binary::size() const noexcept {
    if (impl_ == nullptr) {
        return 0;
    }
    return impl_->data.size();
}

std::size_t shared_binary::use_count() const noexcept {
    if (impl_ == nullptr) {
        return 0;
    }
    return impl_->ref.load();
}

void shared_binary::reset() noexcept {
    if (impl_ != nullptr) {
        const std::size_t post_ref = --(impl_->ref);
        if (post_ref == 0) {
            delete impl_;
        }
        impl_ = nullptr;
    }
}

bool operator==(
    const shared_binary& left, const shared_binary& right) noexcept {
    if (!left.has_data()) {
        return !right.has_data();
    }
    if (!right.has_data()) {
        return false;
    }
    return std::equal(left.data(), left.data() + left.size(), right.data(),
        right.data() + right.size());
}

bool operator!=(
    const shared_binary& left, const shared_binary& right) noexcept {
    return !(left == right);
}

}  // namespace mprpc
