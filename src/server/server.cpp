#include "server.hpp"
#include "socket_fd.hpp"

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

Server::Server(int port, const std::string& config_path)
    : scanner_(config_path), stats_(scanner_.get_known_patterns().size()) {

    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd_ < 0) {
        throw std::runtime_error("Server Error: Cannot create socket");
    }

    int opt = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd_, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) <
        0) {
        close(server_fd_);
        throw std::runtime_error("Server Error: Bind failed");
    }

    if (listen(server_fd_, SOMAXCONN) < 0) {
        close(server_fd_);
        throw std::runtime_error("Server Error: Listen failed");
    }
}

Server::~Server() {
    if (server_fd_ >= 0) {
        close(server_fd_);
    }
}

void Server::start() {
    std::cout << "[Server] Started \nListening for connections: \n";

    while (is_running.load()) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        int client_fd =
            accept(server_fd_, reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);

        if (client_fd < 0) {
            if (errno == EINTR) {
                continue;
            }

            std::cerr << "Server Error: accept failed\n";
            continue;
        }

        pid_t pid = fork();

        if (pid < 0) {
            std::cerr << "Server Error: fork failed\n";
            close(client_fd);
        } else if (pid == 0) {
            close(server_fd_);
            handle_client(client_fd);
            exit(0);
        } else {
            close(client_fd);
        }
    }

    print_final_statistics();
}

void Server::handle_client(int client_fd) {
    SocketFD client_sock(client_fd);

    std::string content;
    char buffer[65536];
    ssize_t bytes_read;

    while ((bytes_read = recv(client_sock.get(), buffer, sizeof(buffer), 0)) > 0) {
        content.append(buffer, bytes_read);
    }

    if (bytes_read < 0) {
        std::cerr << "Worker Error: failed to read from socket\n";
        return;
    }

    ScanResult result = scanner_.scan_content(content);
    stats_.add_file_checked();

    std::string response = "OK: Clean";
    if (result.is_infected) {
        response = "DANGER: Infected\nFound: ";
        const auto& patterns = scanner_.get_known_patterns();

        for (size_t i = 0; i < patterns.size(); ++i) {
            auto it = result.threats_found.find(patterns[i]);
            if (it != result.threats_found.end()) {
                stats_.add_threat_count(i, it->second);
                response += patterns[i] + "(" + std::to_string(it->second) + ") ";
            }
        }
    }

    send(client_sock.get(), response.c_str(), response.length(), 0);
}

void Server::print_final_statistics() const {
    std::cout << "\n----------- FINAL SERVER STATISTICS -----------\n";
    std::cout << "Total files checked: " << stats_.get_files_checked() << "\n";

    const auto& patterns = scanner_.get_known_patterns();
    for (size_t i = 0; i < patterns.size(); ++i) {
        size_t count = stats_.get_threat_count(i);
        if (count > 0) {
            std::cout << "Threat '" << patterns[i] << "' found " << count << " times.\n";
        }
    }
    std::cout << "--------------------------------------------\n";
}