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

int Process::Pid() const { return id_; }

float Process::CpuUtilization() const {
  auto total = LinuxParser::UpTime(Pid());
  auto active = LinuxParser::ActiveJiffies(Pid()) / sysconf(_SC_CLK_TCK);
  return static_cast<float>(active) / total;
}

string Process::Command() const { return LinuxParser::Command(Pid()); }

string Process::Ram() const { return LinuxParser::Ram(Pid()); }

string Process::User() const { return LinuxParser::User(Pid()); }

long int Process::UpTime() const { return LinuxParser::UpTime(Pid()); }

int Process::operator<=>(Process const& a) const {
  auto p1 = CpuUtilization();
  auto p2 = a.CpuUtilization();
  return p1 == p2 ? 0 : p1 < p2 ? -1 : 1;
}
