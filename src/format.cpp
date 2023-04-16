#include "format.h"

#include <sstream>
#include <string>

using std::string;

string Format::ElapsedTime(long seconds) {
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  int remaining_seconds = seconds % 60;
  std::stringstream stream;
  stream << hours << ":" << minutes << ":" << remaining_seconds;
  return stream.str();
}