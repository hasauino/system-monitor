#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <chrono>
using namespace std::chrono;

class Processor {
 public:
  Processor(milliseconds sampling_time = 1000ms);
  float Utilization();

 private:
  void Sample();
  long total_{0};
  long idle_{0};
  float total_diff_{0};
  float idle_diff_{0};
  milliseconds sampling_time_{1000};
  time_point<steady_clock> timestamp_;
};

#endif