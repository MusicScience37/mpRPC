add_executable(mprpc_bench_transport_binary raw_tcp_test.cpp raw_udp_test.cpp
                                            msgpack_tcp_test.cpp)
add_mprpc_bench(mprpc_bench_transport_binary)
