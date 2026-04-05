#pragma once

class SocketFD {
public:
    SocketFD();
    ~SocketFD();

    explicit SocketFD(int fd) : fd_(fd) {
    }

    SocketFD(const SocketFD&) = delete;
    SocketFD& operator=(const SocketFD&) = delete;

    SocketFD(SocketFD&& other) noexcept;
    SocketFD& operator=(SocketFD&& other) noexcept;

    int get() const;
    void shutdown_write();

private:
    int fd_{-1};
};