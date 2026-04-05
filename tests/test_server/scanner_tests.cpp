#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include <stdexcept>
#include "scanner.hpp"

class ScannerTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::ofstream out(test_config_path);

        if (!out.is_open() || out.fail()) {
            throw std::runtime_error("Tests Error: critical failure");
        }

        out << "DANGER_EXEC\n";
        out << "VIRUS_INJECT\n";
        out.close();
    }

    void TearDown() override {
        std::remove(test_config_path.c_str());
    }

    std::string test_config_path = "temp_test_config.txt";
};

TEST_F(ScannerTest, HandlesCleanFile) {
    ScannerOfThreats scanner(test_config_path);

    std::string safe_text =
        "This is a normal server log, nothing suspicious. Some lines 1 2 3 45 ask";
    ScanResult result = scanner.scan_content(safe_text);

    EXPECT_FALSE(result.is_infected);
    EXPECT_TRUE(result.threats_found.empty());
}

TEST_F(ScannerTest, FindsSingleThreat) {
    ScannerOfThreats scanner(test_config_path);

    std::string bad_text = "Some good text but we have DANGER_EXEC some fake news";
    ScanResult result = scanner.scan_content(bad_text);

    EXPECT_TRUE(result.is_infected);
    EXPECT_EQ(result.threats_found.size(), 1);
    EXPECT_EQ(result.threats_found["DANGER_EXEC"], 1);
}

TEST_F(ScannerTest, CountsMultipleThreats) {
    ScannerOfThreats scanner(test_config_path);

    std::string very_bad_text = "DANGER_EXEC here || VIRUS_INJECT here, and one more DANGER_EXEC!";
    ScanResult result = scanner.scan_content(very_bad_text);

    EXPECT_TRUE(result.is_infected);
    EXPECT_EQ(result.threats_found.size(), 2);
    EXPECT_EQ(result.threats_found["DANGER_EXEC"], 2);
    EXPECT_EQ(result.threats_found["VIRUS_INJECT"], 1);
}

TEST_F(ScannerTest, ThrowsOnMissingConfig) {
    EXPECT_THROW(
        { ScannerOfThreats scanner("fake_file_123456789009123.txt"); }, std::runtime_error);
}