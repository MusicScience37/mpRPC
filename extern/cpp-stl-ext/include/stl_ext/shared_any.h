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
 * \brief declaration and implementation of shared_any class
 */
#pragma once

#include <atomic>
#include <new>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace stl_ext {

namespace impl {

/*!
 * \brief class of data in shared_any class
 */
struct shared_any_data {
    //! pointer to object
    void* obj_ptr{nullptr};

    //! pointer to data to manage objects
    void* mng_ptr{nullptr};
};

/*!
 * \brief enumeration of operations to perform in shared_any_impl class
 */
enum class shared_any_impl_operation {
    get_ptr,        //! get pointer to object
    get_type,       //! get type of object
    get_ref_count,  //! get reference count
    add_ref,        //! add reference count
    release         //! release pointer
};

/*!
 * \brief internal implementation for shared_any class
 *
 * \tparam Object type of object
 */
template <typename Object>
class shared_any_impl {
public:
    /*!
     * \brief struct of data to manage objects
     */
    struct manager_data {
        //! reference count
        std::atomic<unsigned int> ref_count{1};
    };

    /*!
     * \brief create object
     *
     * \tparam Args type of arguments
     * \param output output
     * \param args arguments
     */
    template <typename... Args>
    static void create(shared_any_data& output, Args&&... args) {  // NOLINT
        // clang cannot find corresponding "delete" statement and output
        // false-positive warnings, so silence them here.
#ifndef __clang_analyzer__
        output.obj_ptr = new (std::nothrow) Object(std::forward<Args>(args)...);
        output.mng_ptr = new (std::nothrow) manager_data();
#endif
        if (output.obj_ptr == nullptr || output.mng_ptr == nullptr) {
            throw std::bad_alloc();
        }
    }

    /*!
     * \brief destroy object
     *
     * \param data data
     */
    static void destroy(shared_any_data& data) noexcept {
        delete static_cast<Object*>(data.obj_ptr);
        delete static_cast<manager_data*>(data.mng_ptr);
        data.obj_ptr = nullptr;
        data.mng_ptr = nullptr;
    }

    /*!
     * \brief manage object
     *
     * \param op operation to perform
     * \param input input
     * \param output output
     * \param ref_count pointer to write reference count in get_ref_count
     *                  operation
     * \return additional output
     */
    static void* manage(shared_any_impl_operation op,
        const shared_any_data& input, shared_any_data& output,
        unsigned int* ref_count) noexcept {
        const auto* input_mng = static_cast<manager_data*>(input.mng_ptr);
        auto* output_mng = static_cast<manager_data*>(output.mng_ptr);

        switch (op) {
        case shared_any_impl_operation::get_ptr:
            return input.obj_ptr;
        case shared_any_impl_operation::get_type:
            // NOLINTNEXTLINE: to use the same API
            return const_cast<void*>(static_cast<const void*>(&typeid(Object)));
        case shared_any_impl_operation::get_ref_count:
            *ref_count = input_mng->ref_count.load();
            break;
        case shared_any_impl_operation::add_ref:
            ++(output_mng->ref_count);
            break;
        case shared_any_impl_operation::release:
            const auto ref_count = --(output_mng->ref_count);
            if (ref_count == 0) {
                destroy(output);
            }
            break;
        }
        return nullptr;
    }
};

}  // namespace impl

/*!
 * \brief class to indicate shared_any object is null
 */
class null_shared_any_t {};

/*!
 * \brief exception for incorrect cast in shared_any class
 */
class bad_shared_any_cast : public std::bad_cast {
    using std::bad_cast::bad_cast;
};

/*!
 * \brief class to save any objects and share as shared_ptr
 */
class shared_any {
public:
    //! construct
    shared_any() = default;

    /*!
     * \brief construct
     *
     * \tparam T object type
     * \param obj object
     */
    template <typename T,
        typename =
            std::enable_if_t<!std::is_same<std::decay_t<T>, shared_any>::value>>
    shared_any(T&& obj) {  // NOLINT: use implicit convertion
        using object_type = std::decay_t<T>;
        using impl_type = impl::shared_any_impl<object_type>;
        impl_type::create(data_, std::forward<T>(obj));
        manager_ = &impl_type::manage;
    }

    /*!
     * \brief copy
     *
     * \param obj another object
     */
    shared_any(const shared_any& obj) noexcept
        : data_(obj.data_), manager_(obj.manager_) {
        if (manager_ == nullptr) {
            return;
        }
        manager_(
            impl::shared_any_impl_operation::add_ref, data_, data_, nullptr);
    }

    /*!
     * \brief copy
     *
     * \param obj another object
     * \return this object
     */
    shared_any& operator=(const shared_any& obj) noexcept {
        if (this == &obj) {
            return *this;
        }
        reset();
        data_ = obj.data_;
        manager_ = obj.manager_;
        if (manager_ == nullptr) {
            return *this;
        }
        manager_(
            impl::shared_any_impl_operation::add_ref, data_, data_, nullptr);
        return *this;
    }

    /*!
     * \brief move
     *
     * \param obj another object
     */
    shared_any(shared_any&& obj) noexcept { swap(obj); }

    /*!
     * \brief move
     *
     * \param obj another object
     * \return this object
     */
    shared_any& operator=(shared_any&& obj) noexcept {
        swap(obj);
        return *this;
    }

    /*!
     * \brief assign an object
     *
     * \tparam T type of object
     * \param obj object
     * \return this object
     */
    template <typename T,
        typename =
            std::enable_if_t<!std::is_same<std::decay_t<T>, shared_any>::value>>
    shared_any& operator=(T&& obj) {
        emplace<std::decay_t<T>>(std::forward<T>(obj));
        return *this;
    }

    //! destruct
    ~shared_any() { reset(); }

    /*!
     * \brief create an object inside this object
     *
     * \tparam ObjectType type object
     * \tparam Args types of arguments
     * \param args arguments
     * \return created object
     */
    template <typename ObjectType, typename... Args>
    std::decay_t<ObjectType>& emplace(Args&&... args) {
        reset();

        using object_type = std::decay_t<ObjectType>;
        using impl_type = impl::shared_any_impl<object_type>;
        impl_type::create(data_, std::forward<Args>(args)...);
        manager_ = &impl_type::manage;

        return get<object_type>();
    }

    /*!
     * \brief swap
     *
     * \param obj another object
     */
    void swap(shared_any& obj) noexcept {
        std::swap(data_, obj.data_);
        std::swap(manager_, obj.manager_);
    }

    /*!
     * \brief get whether this has an object
     *
     * \return whether this has an object
     */
    bool has_value() const noexcept { return manager_ != nullptr; }

    /*!
     * \brief get type of object
     *
     * \return type of object
     */
    const std::type_info& type() const noexcept {
        if (manager_ == nullptr) {
            return typeid(null_shared_any_t);
        }
        impl::shared_any_data ignored;
        const auto* ptr = manager_(
            impl::shared_any_impl_operation::get_type, data_, ignored, nullptr);
        return *static_cast<const std::type_info*>(ptr);
    }

    /*!
     * \brief get void pointer
     *
     * \return void pointer
     */
    void* void_ptr() const noexcept {
        if (manager_ == nullptr) {
            return nullptr;
        }
        impl::shared_any_data ignored;
        return manager_(
            impl::shared_any_impl_operation::get_ptr, data_, ignored, nullptr);
    }

    /*!
     * \brief get object
     *
     * \tparam T type
     * \return object
     */
    template <typename T>
    T& get() {
        if (type() != typeid(T) || !has_value()) {
            throw bad_shared_any_cast();
        }
        return *static_cast<T*>(void_ptr());
    }

    /*!
     * \brief get object
     *
     * \tparam T type
     * \return object
     */
    template <typename T>
    const T& get() const {
        if (type() != typeid(T) || !has_value()) {
            throw bad_shared_any_cast();
        }
        return *static_cast<const T*>(void_ptr());
    }

    /*!
     * \brief get reference count
     *
     * \return reference count
     */
    long use_count() const noexcept {  // NOLINT: for consistency with std
        if (manager_ == nullptr) {
            return 0;
        }
        unsigned int ref_count = 0;
        impl::shared_any_data ignored;
        manager_(impl::shared_any_impl_operation::get_ref_count, data_, ignored,
            &ref_count);
        return ref_count;
    }

    /*!
     * \brief clear this object to null
     */
    void reset() noexcept {
        if (manager_ == nullptr) {
            return;
        }
        manager_(
            impl::shared_any_impl_operation::release, data_, data_, nullptr);
        manager_ = nullptr;
    }

private:
    //! type of manager
    using manager_type = void* (*)(impl::shared_any_impl_operation,
        const impl::shared_any_data&, impl::shared_any_data&, unsigned int*);

    //! data
    impl::shared_any_data data_{};

    //! manager
    manager_type manager_{nullptr};
};

}  // namespace stl_ext

namespace std {

/*!
 * \brief specialization of std::swap for stl_ext::shared_any class
 *
 * \param obj1 an object
 * \param obj2 an object
 */
template <>
inline void swap<stl_ext::shared_any>(
    stl_ext::shared_any& obj1, stl_ext::shared_any& obj2) noexcept {
    obj1.swap(obj2);
}

}  // namespace std
