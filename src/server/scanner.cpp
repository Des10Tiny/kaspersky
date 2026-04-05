#include "scanner.hpp"

#include <fstream>
#include <stdexcept>

ScannerOfThreats::ScannerOfThreats(const std::string& config_path) {
    std::ifstream file(config_path);

    if (!file.is_open()) {
        throw std::runtime_error("Server Error: Cannot open config file " + config_path);
    }

    std::string line;

    while (std::getline(file, line)) {
        line.erase(line.find_last_not_of(" \n\r\t") + 1);
        line.erase(0, line.find_first_not_of(" \n\r\t"));

        if (!line.empty()) {
            patterns_.push_back(line);
        }
    }

    if (patterns_.empty()) {
        throw std::runtime_error("Server Error: config file is invalid");
    }
}

ScanResult ScannerOfThreats::scan_content(const std::string& content) const {
    ScanResult result;

    for (const auto& pattern : patterns_) {
        size_t pos = 0;
        int count = 0;

        while ((pos = content.find(pattern, pos)) != std::string::npos) {
            count++;
            pos += pattern.length();
        }

        if (count > 0) {
            result.is_infected = true;
            result.threats_found[pattern] = count;
        }
    }

    return result;
}

const std::vector<std::string>& ScannerOfThreats::get_known_patterns() const {
    return patterns_;
}