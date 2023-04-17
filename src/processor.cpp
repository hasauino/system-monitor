#include "processor.h"

#include <chrono>

#include "linux_parser.h"

using namespace std::chrono;

Processor::Processor(milliseconds sampling_time)
    : sampling_time_{sampling_time}, timestamp_(steady_clock::now()) {
  total_ = LinuxParser::Jiffies();
  idle_ = LinuxParser::IdleJiffies();
}
float Processor::Utilization() {
  Sample();
  if (total_diff_ == 0) return 0;
  return (total_diff_ - idle_diff_) / total_diff_;
}

void Processor::Sample() {
  auto duration = steady_clock::now() - timestamp_;
  if (duration < sampling_time_) return;
  auto total_now = LinuxParser::Jiffies();
  auto idle_now = LinuxParser::IdleJiffies();
  total_diff_ = total_now - total_;
  idle_diff_ = idle_now - idle_;
  total_ = LinuxParser::Jiffies();
  idle_ = LinuxParser::IdleJiffies();
  timestamp_ = steady_clock::now();
}