#include "shared_stats.hpp"

#include <sys/mman.h>
#include <unistd.h>
#include <stdexcept>

SharedStats::SharedStats(size_t num_patterns) : num_patterns_(num_patterns) {
    size_bytes_ = sizeof(std::atomic<size_t>) * (num_patterns_ + 1);

    mapped_memory_ =
        mmap(nullptr, size_bytes_, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (mapped_memory_ == MAP_FAILED) {
        throw std::runtime_error("Server Error: Failed to allocate shared memory via mmap");
    }

    total_files_ = new (mapped_memory_) std::atomic<size_t>(0);
    patterns_array_ = total_files_ + 1;

    for (size_t i = 0; i < num_patterns_; ++i) {
        new (&patterns_array_[i]) std::atomic<size_t>(0);
    }
}

SharedStats::~SharedStats() {
    if (mapped_memory_ != MAP_FAILED) {
        munmap(mapped_memory_, size_bytes_);
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