#include <memory>
#include <thread>

#include <asio/ip/udp.hpp>

#include "common.h"
#include "mprpc/generate_binary.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/logging/spdlog_logger.h"
#include "mprpc/thread_pool.h"

constexpr std::size_t max_length = 65535;

class udp_server {
public:
    udp_server(std::shared_ptr<mprpc::logging::logger> logger,
        const asio::ip::udp::endpoint& endpoint, asio::io_context& context)
        : logger_(std::move(logger)), socket_(context, endpoint) {}

    void start() { do_read(); }

private:
    void do_read() {
        socket_.async_receive_from(asio::buffer(data_.data(), data_.size()),
            sender_,
            [this](const asio::error_code& error, std::size_t num_bytes) {
                this->on_read(error, num_bytes);
            });
    }

    void on_read(const asio::error_code& error, std::size_t num_bytes) {
        if (error) {
            if (error == asio::error::operation_aborted) {
                return;
            }
            MPRPC_ERROR(logger_, "server error: {}", error.message());
            return;
        }
        socket_.async_send_to(asio::buffer(data_.data(), num_bytes), sender_,
            [this](const asio::error_code& /*error*/,
                std::size_t /*num_bytes*/) { this->do_read(); });
    }

    std::shared_ptr<mprpc::logging::logger> logger_;

    asio::ip::udp::socket socket_;

    asio::ip::udp::endpoint sender_{};

    std::vector<char> data_{std::vector<char>(max_length)};
};

static void transport_binary_raw_udp(benchmark::State& state) {
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
        asio::ip::udp::endpoint(asio::ip::address_v4::loopback(), port);

    try {
        auto server = udp_server(logger, endpoint, threads->context());
        server.start();

        const auto wait_duration = std::chrono::milliseconds(100);
        std::this_thread::sleep_for(wait_duration);

        auto client = asio::ip::udp::socket(threads->context(),
            asio::ip::udp::endpoint(asio::ip::address_v4::loopback(), 0));

        for (auto _ : state) {
            client.send_to(asio::buffer(data.data(), data.size()), endpoint);
            client.receive(asio::buffer(read_data.data(), read_data.size()));
        }

        threads->stop();
    } catch (const asio::system_error& err) {
        MPRPC_ERROR(logger, err.what());
        state.SkipWithError(err.what());
    }

    set_counters(state);
}
BENCHMARK(transport_binary_raw_udp)
    ->UseRealTime()
    ->RangeMultiplier(32)   // NOLINT
    ->Range(1, 1024 * 32);  // NOLINT
