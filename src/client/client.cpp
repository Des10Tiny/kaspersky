#include "client.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <arpa/inet.h>

Client::Client(const std::string& ip, int port) {
    connect_to_server(ip, port);
}

void Client::send_file(const std::string& file_path) {
    std::string content = read_file_content(file_path);
    size_t total_sent = 0;
    const char* data_ptr = content.data();
    size_t bytes_left = content.size();

    while (bytes_left > 0) {
        ssize_t sent = send(sock_.get(), data_ptr + total_sent, bytes_left, 0);
        if (sent < 0) {
            throw std::runtime_error("Error: send to server wasn't complete");
        }
        total_sent += sent;
        bytes_left -= sent;
    }

    sock_.shutdown_write();
}

std::string Client::receive_result() {
    std::string result;
    char buffer[1024];
    ssize_t bytes_received;

    while ((bytes_received = recv(sock_.get(), buffer, sizeof(buffer) - 1, 0)) > 0) {
        result.append(buffer, bytes_received);
    }

    if (bytes_received < 0) {
        throw std::runtime_error("Error: server doesn't response");
    }

    return result;
}

void Client::connect_to_server(const std::string& ip, int port) {
    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0) {
        throw std::runtime_error("Ip is invalid");
    }

    if (connect(sock_.get(), reinterpret_cast<struct sockaddr*>(&serv_addr), sizeof(serv_addr)) <
        0) {
        throw std::runtime_error("Error: cannot connect to server || maybe port is invalid");
    }
}

std::string Client::read_file_content(const std::string& file_path) const {
    std::ifstream file(file_path);

    if (!file.is_open()) {
        throw std::runtime_error("Error: file cannot be opened " + file_path);
    }

    std::ostringstream ss;
    ss << file.rdbuf();

    return ss.str();
}
