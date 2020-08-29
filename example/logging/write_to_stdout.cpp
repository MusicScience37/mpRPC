#include <mprpc/logging/logging_macros.h>
#include <mprpc/logging/spdlog_logger.h>

int main() {
    auto logger = mprpc::logging::create_stdout_logger(
        "test_logger", mprpc::logging::log_level::trace);

    MPRPC_TRACE(logger, "test");
    MPRPC_DEBUG(logger, "test");
    MPRPC_INFO(logger, "test");
    MPRPC_WARN(logger, "test");
    MPRPC_ERROR(logger, "test");
    MPRPC_FATAL(logger, "test");
}
