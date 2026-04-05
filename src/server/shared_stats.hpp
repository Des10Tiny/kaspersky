#pragma once

#include <atomic>
#include <cstddef>

class SharedStats {
public:
    explicit SharedStats(size_t num_patterns);
    ~SharedStats();

    SharedStats(const SharedStats&) = delete;
    SharedStats& operator=(const SharedStats&) = delete;
    SharedStats(SharedStats&&) = delete;
    SharedStats& operator=(SharedStats&&) = delete;

    void add_file_checked();
    void add_threat_count(size_t pattern_index, size_t count);

    size_t get_files_checked() const;
    size_t get_threat_count(size_t pattern_index) const;

private:
    size_t num_patterns_;
    size_t size_bytes_;
    void* mapped_memory_{nullptr};

    std::atomic<size_t>* total_files_;
    std::atomic<size_t>* patterns_array_;
};