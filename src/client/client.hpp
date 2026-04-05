#pragma once

#include <string>
#include "socket_fd.hpp"

class Client {
public:
    Client(const std::string& ip, int port);

    void send_file(const std::string& file_path);
    std::string receive_result();

private:
    SocketFD sock_;

    void connect_to_server(const std::string& ip, int port);
    std::string read_file_content(const std::string& file_path) const;
};