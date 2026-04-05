#include <gtest/gtest.h>
#include <sys/wait.h>
#include <unistd.h>
#include "shared_stats.hpp"

TEST(SharedStatsTest, BasicInitialization) {
    SharedStats stats(3);

    EXPECT_EQ(stats.get_files_checked(), 0);
    EXPECT_EQ(stats.get_threat_count(0), 0);

    stats.add_file_checked();
    stats.add_threat_count(1, 5);

    EXPECT_EQ(stats.get_files_checked(), 1);
    EXPECT_EQ(stats.get_threat_count(1), 5);
    EXPECT_EQ(stats.get_threat_count(2), 0);
}

TEST(SharedStatsTest, ForkSharedMemory) {
    SharedStats stats(2);

    pid_t pid = fork();
    ASSERT_GE(pid, 0);

    if (pid == 0) {
        stats.add_file_checked();
        stats.add_threat_count(0, 42);
        exit(0);
    } else {
        int status;
        waitpid(pid, &status, 0);

        EXPECT_EQ(stats.get_files_checked(), 1);
        EXPECT_EQ(stats.get_threat_count(0), 42);
    }
}