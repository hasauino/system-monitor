
#include <gtest/gtest.h>

#include <cmath>
#include <iostream>
#include <vector>

#include "linux_parser.h"

TEST(LinuxParser, OperatingSystem) {
  EXPECT_EQ(LinuxParser::OperatingSystem("tests/assets/os-release"),
            "Ubuntu 22.04.2 LTS");
}

TEST(LinuxParser, KernelVersion) {
  EXPECT_EQ(LinuxParser::Kernel("tests/assets/version"), "5.19.0-38-generic");
}

TEST(LinuxParser, MemoryUtilization) {
  EXPECT_NEAR(LinuxParser::MemoryUtilization("tests/assets/meminfo"), 0.4817,
              0.0001);
}

TEST(LinuxParser, UptTime) {
  EXPECT_EQ(LinuxParser::UpTime("tests/assets/uptime"), 71339);
}

TEST(LinuxParser, Jiffies) {
  EXPECT_EQ(LinuxParser::Jiffies("tests/assets/stat"), 8079402);
}

TEST(LinuxParser, ActiveJiffies) {
  EXPECT_EQ(LinuxParser::ActiveJiffies("tests/assets/stat"), 1108906);
}

TEST(LinuxParser, IdleJiffies) {
  EXPECT_EQ(LinuxParser::IdleJiffies("tests/assets/stat"), 6970496);
}

TEST(LinuxParser, TotalProcesses) {
  EXPECT_EQ(LinuxParser::TotalProcesses("tests/assets/stat"), 97366);
}

TEST(LinuxParser, RunningProcesses) {
  EXPECT_EQ(LinuxParser::RunningProcesses("tests/assets/stat"), 1);
}

TEST(LinuxParser, Pids) {
  std::vector expected{12944, 56230};
  EXPECT_EQ(LinuxParser::Pids("tests/assets/processes/"), expected);
}

TEST(LinuxParser, Command) {
  EXPECT_EQ(LinuxParser::Command(12944, "tests/assets/processes/"),
            "/usr/bin/nautilus\0--gapplication-service\0");
}
