#pragma once

#include <string>
#include <atomic>
#include "scanner.hpp"
#include "shared_stats.hpp"

class Server {
public:
    Server(int port, const std::string& config_path);
    ~Server();

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(Server&&) = delete;

    void start();

    inline static std::atomic<bool> is_running{true};

private:
    void handle_client(int client_fd);
    void print_final_statistics() const;

    int server_fd_{-1};
    ScannerOfThreats scanner_;
    SharedStats stats_;
};