#include "app/CliParser.hpp"
#include "app/ServerFactory.hpp"

#include <asio/io_context.hpp>
#include <asio/signal_set.hpp>
#include <csignal>
#include <thread>
#include <vector>

int main(int argc, char* argv[]) {
    const CliParser parser;
    const CliOptions opts = parser.parse(argc, argv);

    asio::io_context io_ctx;
    ServerFactory factory(io_ctx, opts.port);
    const auto server = factory.create(opts.api);

    asio::signal_set signals(io_ctx, SIGINT, SIGTERM);
    signals.async_wait([&server, &io_ctx](const std::error_code&, int) {
        server->Stop();
        io_ctx.stop();
    });

    server->Start();

    std::vector<std::jthread> workers;
    workers.reserve(opts.threads);
    for (unsigned i = 0; i < opts.threads; ++i) {
        workers.emplace_back([&io_ctx] { io_ctx.run(); });
    }
    return 0;
}
