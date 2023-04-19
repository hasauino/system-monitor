#include "format.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using std::string;

string Format::ElapsedTime(long seconds) {
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  int remaining_seconds = seconds % 60;
  std::stringstream stream;
  stream << std::setw(2) << std::setfill('0') << hours << ":" << std::setw(2)
         << std::setfill('0') << minutes << ":" << std::setw(2)
         << std::setfill('0') << remaining_seconds;
  return stream.str();
}