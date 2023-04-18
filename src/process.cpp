#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int id) : id_{id} {}

int Process::Pid() { return id_; }

float Process::CpuUtilization() {
  auto total = LinuxParser::UpTime(Pid());
  auto active = LinuxParser::ActiveJiffies(Pid()) / sysconf(_SC_CLK_TCK);
  return static_cast<float>(active) / total;
}

string Process::Command() { return LinuxParser::Command(Pid()); }

string Process::Ram() { return LinuxParser::Ram(Pid()); }

string Process::User() { return LinuxParser::User(Pid()); }

long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a [[maybe_unused]]) const {
  return true;
}