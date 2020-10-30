#include <future>
#include <iostream>
#include <memory>
#include <thread>

#include "common.h"
#include "mprpc/generate_binary_data.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/logging/spdlog_logger.h"
#include "mprpc/thread_pool.h"
#include "mprpc/transport/parsers/msgpack_parser.h"
#include "mprpc/transport/tcp/tcp.h"

class mprpc_session : public std::enable_shared_from_this<mprpc_session> {
public:
    explicit mprpc_session(std::shared_ptr<mprpc::transport::session> session)
        : session_(std::move(session)) {}

    void start() { do_read(); }

private:
    void do_read() {
        session_->async_read(
            [weak_self = weak_from_this()](const mprpc::error_info& error,
                const mprpc::message_data& message) {
                auto self = weak_self.lock();
                if (!self) {
                    return;
                }
                self->on_read(error, message);
            });
    }

    void on_read(
        const mprpc::error_info& error, const mprpc::message_data& message) {
        if (error) {
            return;
        }

        session_->async_write(message,
            [weak_self = weak_from_this()](const mprpc::error_info& error) {
                auto self = weak_self.lock();
                if (!self) {
                    return;
                }
                self->on_write(error);
            });
    }

    void on_write(const mprpc::error_info& error) {
        if (error) {
            return;
        }

        do_read();
    }

    std::weak_ptr<mprpc_session> weak_from_this() {
        return std::enable_shared_from_this<mprpc_session>::shared_from_this();
    }

    std::shared_ptr<mprpc::transport::session> session_;
};

class mprpc_server {
public:
    explicit mprpc_server(std::shared_ptr<mprpc::transport::acceptor> acceptor)
        : acceptor_(std::move(acceptor)) {}

    void start() { do_accept(); }

    void close() { acceptor_.reset(); }

private:
    void do_accept() {
        acceptor_->async_accept(
            [this](const mprpc::error_info& error,
                const std::shared_ptr<mprpc::transport::session>& session) {
                this->on_accept(error, session);
            });
    }

    void on_accept(const mprpc::error_info& error,
        const std::shared_ptr<mprpc::transport::session>& session) {
        if (error) {
            return;
        }
        auto s = std::make_shared<mprpc_session>(session);
        s->start();
        sessions_.push_back(s);
        do_accept();
    }

    std::shared_ptr<mprpc::transport::acceptor> acceptor_;

    std::vector<std::shared_ptr<mprpc_session>> sessions_;
};

static void transport_binary_msgpack_tcp(benchmark::State& state) {
    static const auto logger = mprpc::logging::create_stdout_logger(
        "mprpc_bench_transport_binary_msgpack_tcp",
        mprpc::logging::log_level::error);

    constexpr std::size_t num_threads = 2;
    const auto threads =
        std::make_shared<mprpc::thread_pool>(logger, num_threads);
    threads->start();

    const auto size = static_cast<std::size_t>(state.range());
    const auto data = mprpc::generate_binary_data(size);

    const auto host = std::string("127.0.0.1");
    constexpr std::uint16_t port = 3780;

    const auto parser_factory =
        std::make_shared<mprpc::transport::parsers::msgpack_parser_factory>();

    try {
        auto acceptor = mprpc::transport::tcp::create_tcp_acceptor(
            logger, host, port, *threads, parser_factory);
        auto server = std::make_shared<mprpc_server>(acceptor);
        server->start();

        const auto wait_duration = std::chrono::milliseconds(100);
        std::this_thread::sleep_for(wait_duration);

        auto client = mprpc::transport::tcp::create_tcp_connector(
            logger, host, port, *threads, parser_factory);

        for (auto _ : state) {
            {
                auto promise = std::promise<void>();
                auto future = promise.get_future();
                client->async_write(
                    data, [&promise](const mprpc::error_info& error) {
                        if (error) {
                            promise.set_exception(std::make_exception_ptr(
                                mprpc::exception(error)));
                        } else {
                            promise.set_value();
                        }
                    });
                future.get();
            }
            {
                auto promise = std::promise<mprpc::message_data>();
                auto future = promise.get_future();
                client->async_read([&promise](const mprpc::error_info& error,
                                       const mprpc::message_data& message) {
                    if (error) {
                        promise.set_exception(
                            std::make_exception_ptr(mprpc::exception(error)));
                    } else {
                        promise.set_value(message);
                    }
                });
                future.get();
            }
        }

        threads->stop();
    } catch (const std::exception& err) {
        MPRPC_ERROR(logger, err.what());
        state.SkipWithError(err.what());
    }

    set_counters(state);
}
BENCH_TRANSPORT(transport_binary_msgpack_tcp);
