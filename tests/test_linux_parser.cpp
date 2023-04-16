
#include <gtest/gtest.h>

#include <cmath>
#include <iostream>

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
