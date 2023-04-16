
#include <gtest/gtest.h>

#include <cmath>
#include <iostream>

#include "format.h"

TEST(Format, ElapsedTime) { EXPECT_EQ(Format::ElapsedTime(71339), "19:48:59"); }

//int main() { std::cout << sysconf(_SC_CLK_TCK) << std::endl; }
