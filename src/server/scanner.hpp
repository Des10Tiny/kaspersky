#pragma once

#include <string>
#include <vector>
#include <unordered_map>

struct ScanResult {
    bool is_infected{false};
    std::unordered_map<std::string, int> threats_found;
};

class ScannerOfThreats {
public:
    explicit ScannerOfThreats(const std::string& config_path);

    ScanResult scan_content(const std::string& content) const;

    const std::vector<std::string>& get_known_patterns() const;

private:
    std::vector<std::string> patterns_;
};