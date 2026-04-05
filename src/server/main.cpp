#include "server.hpp"
#include <iostream>
#include <csignal>
#include <cstdlib>

void signal_handler(int signum) {
    if (signum == SIGINT || signum == SIGTERM) {
        Server::is_running = false;
    }
}

void setup_signals() {
    struct sigaction sa{};
    sa.sa_handler = signal_handler;
    sigaction(SIGINT, &sa, nullptr);
    sigaction(SIGTERM, &sa, nullptr);

    struct sigaction sa_chld{};
    sa_chld.sa_handler = SIG_IGN;
    sa_chld.sa_flags = SA_NOCLDWAIT;
    sigaction(SIGCHLD, &sa_chld, nullptr);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <config_file>\n";
        return EXIT_FAILURE;
    }

    int port = std::stoi(argv[1]);
    std::string config_path = argv[2];

    setup_signals();

    try {
        Server server(port, config_path);
        server.start();
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}