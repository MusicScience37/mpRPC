add_executable(
    stl_ext_test_units
    # memory-related operations
    allocator_traits_test.cpp
    pod_allocator_test.cpp
    # shared objects
    shared_any_test.cpp
    shared_function_test.cpp
    # containers
    impl/safe_double_size_test.cpp
    impl/vector_iterator_test.cpp
    pod_vector_test.cpp)
target_add_catch2(stl_ext_test_units)
