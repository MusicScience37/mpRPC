#include <memory>
#include <thread>

#include <asio/ip/tcp.hpp>
#include <asio/read.hpp>
#include <asio/write.hpp>

#include "common.h"
#include "mprpc/generate_binary.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/logging/spdlog_logger.h"
#include "mprpc/thread_pool.h"

class session : public std::enable_shared_from_this<session> {
public:
    session(std::shared_ptr<mprpc::logging::logger> logger,
        asio::ip::tcp::socket socket, std::size_t data_size)
        : logger_(std::move(logger)),
          socket_(std::move(socket)),
          data_(std::vector<char>(data_size)) {}

    void start() { do_read(); }

private:
    void do_read() {
        asio::async_read(socket_, asio::buffer(data_.data(), data_.size()),
            [self = shared_from_this()](const asio::error_code& error,
                std::size_t /*bytes_transferred*/) { self->on_read(error); });
    }

    void on_read(const asio::error_code& error) {
        if (error) {
            if (error == asio::error::eof) {
                return;
            }
            MPRPC_ERROR(logger_, error.message());
            return;
        }

        asio::async_write(socket_, asio::buffer(data_.data(), data_.size()),
            [self = shared_from_this()](const asio::error_code& error,
                std::size_t /*bytes_transferred*/) { self->on_write(error); });
    }

    void on_write(const asio::error_code& error) {
        if (error) {
            MPRPC_ERROR(logger_, error.message());
            return;
        }

        do_read();
    }

    std::shared_ptr<mprpc::logging::logger> logger_;

    asio::ip::tcp::socket socket_;

    std::vector<char> data_;
};

class tcp_server {
public:
    tcp_server(std::shared_ptr<mprpc::logging::logger> logger,
        const asio::ip::tcp::endpoint& endpoint,
        const std::shared_ptr<mprpc::thread_pool>& threads,
        std::size_t data_size)
        : logger_(std::move(logger)),
          socket_(threads->context(), endpoint),
          data_size_(data_size) {}

    void start() { do_accept(); }

    void close() { socket_.close(); }

private:
    void do_accept() {
        socket_.async_accept([this](const asio::error_code& error,
                                 asio::ip::tcp::socket socket) {
            this->on_accept(error, std::move(socket));
        });
    }

    void on_accept(
        const asio::error_code& error, asio::ip::tcp::socket socket) {
        if (error) {
            if (error == asio::error::operation_aborted) {
                return;
            }
            MPRPC_ERROR(logger_, error.message());
            return;
        }

        auto s =
            std::make_shared<session>(logger_, std::move(socket), data_size_);
        s->start();

        do_accept();
    }

    std::shared_ptr<mprpc::logging::logger> logger_;

    asio::ip::tcp::acceptor socket_;

    std::size_t data_size_;
};

static void transport_binary_raw_tcp(benchmark::State& state) {
    static const auto logger = mprpc::logging::create_stdout_logger();

    constexpr std::size_t num_threads = 2;
    const auto threads =
        std::make_shared<mprpc::thread_pool>(logger, num_threads);
    threads->start();

    const auto size = static_cast<std::size_t>(state.range());
    const auto data = mprpc::generate_binary(size);
    auto read_data = std::vector<char>(size);

    // server address: 127.0.0.1:3780
    constexpr std::uint16_t port = 3780;
    const auto endpoint =
        asio::ip::tcp::endpoint(asio::ip::address_v4::loopback(), port);

    try {
        auto server = tcp_server(logger, endpoint, threads, size);
        server.start();

        const auto wait_duration = std::chrono::milliseconds(100);
        std::this_thread::sleep_for(wait_duration);

        auto client = asio::ip::tcp::socket(threads->context());
        client.connect(endpoint);

        for (auto _ : state) {
            asio::write(client, asio::buffer(data.data(), data.size()));
            asio::read(
                client, asio::buffer(read_data.data(), read_data.size()));
        }

        threads->stop();
    } catch (const asio::system_error& err) {
        MPRPC_ERROR(logger, err.what());
        state.SkipWithError(err.what());
    }

    set_counters(state);
}
BENCH_TRANSPORT(transport_binary_raw_tcp);
