#include <iostream>
#include <asio.hpp>

int main() {
    asio::io_context io;
    asio::steady_timer timer(io, asio::chrono::seconds(3));

    std::cout << "Waiting..\n";
    timer.async_wait([](const asio::error_code& e) {
            std::cout << "Done: e= " << e.message() << "\n";
    });

    io.run();

    return 0;
}
