#include "socket_fd.hpp"
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

SocketFD::SocketFD() {
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_ < 0) {
        throw std::runtime_error("Socket Error: invalid TCP || TCP not initialized");
    }
}

SocketFD::~SocketFD() {
    if (fd_ >= 0) {
        close(fd_);
    }
}

SocketFD::SocketFD(SocketFD&& other) noexcept : fd_(other.fd_) {
    other.fd_ = -1;
}

SocketFD& SocketFD::operator=(SocketFD&& other) noexcept {
    if (this != &other) {
        if (fd_ >= 0) {
            close(fd_);
        }
        fd_ = other.fd_;
        other.fd_ = -1;
    }
    return *this;
}

int SocketFD::get() const {
    return fd_;
}

void SocketFD::shutdown_write() {
    if (fd_ >= 0) {
        shutdown(fd_, SHUT_WR);
    }
}