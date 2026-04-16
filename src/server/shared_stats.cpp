#include "shared_stats.hpp"

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>

const char* shm_name = "/main_shm";

SharedStats::SharedStats(size_t num_patterns, bool is_server)
    : num_patterns_(num_patterns), is_server_(is_server) {

    size_bytes_ = sizeof(std::atomic<size_t>) * (num_patterns_ + 1);
    creator_pid_ = getpid();

    if (is_server_) {
        shm_fd_ = shm_open(shm_name, O_CREAT | O_RDWR, 0666);

        if (shm_fd_ < 0) {
            throw std::runtime_error("SharedStats Error: failed to shm_open");
        }

        if (ftruncate(shm_fd_, size_bytes_) == -1) {
            throw std::runtime_error("SharedStats Error: failed to ftruncate");
        }
    } else {
        shm_fd_ = shm_open(shm_name, O_RDWR, 0666);

        if (shm_fd_ < 0) {
            throw std::runtime_error("SharedStats Error: Server is not running or memory missing");
        }
    }

    mapped_memory_ = mmap(nullptr, size_bytes_, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_, 0);

    if (mapped_memory_ == MAP_FAILED) {
        throw std::runtime_error("SharedStats Error: Failed to allocate shared memory via mmap");
    }

    total_files_ = reinterpret_cast<std::atomic<size_t>*>(mapped_memory_);
    patterns_array_ = total_files_ + 1;

    if (is_server_) {
        new (total_files_) std::atomic<size_t>(0);
        for (size_t i = 0; i < num_patterns_; ++i) {
            new (&patterns_array_[i]) std::atomic<size_t>(0);
        }
    }
}

SharedStats::~SharedStats() {
    if (mapped_memory_ != MAP_FAILED) {
        munmap(mapped_memory_, size_bytes_);
    }
    if (shm_fd_ >= 0) {
        close(shm_fd_);
    }
    if (is_server_ && getpid() == creator_pid_) {
        shm_unlink(shm_name);
    }
}

void SharedStats::add_file_checked() {
    total_files_->fetch_add(1);
}

void SharedStats::add_threat_count(size_t pattern_index, size_t count) {
    if (pattern_index < num_patterns_) {
        patterns_array_[pattern_index].fetch_add(count);
    }
}

size_t SharedStats::get_files_checked() const {
    return total_files_->load();
}

size_t SharedStats::get_threat_count(size_t pattern_index) const {
    if (pattern_index < num_patterns_) {
        return patterns_array_[pattern_index].load();
    }
    return 0;
}