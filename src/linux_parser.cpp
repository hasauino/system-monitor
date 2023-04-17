#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <array>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem(const char* info_path) {
  string line;
  string key;
  string value;
  std::ifstream filestream;
  if (info_path)
    filestream.open(info_path);
  else
    filestream.open(kOSPath);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel(const char* info_path) {
  string os, kernel, version;
  string line;
  std::ifstream stream;
  if (info_path)
    stream.open(info_path);
  else
    stream.open(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization(const char* info_path) {
  std::ifstream stream;
  if (info_path)
    stream.open(info_path);
  else
    stream.open(kProcDirectory + kMeminfoFilename);
  if (!stream.is_open()) return 0.0;
  std::string line, label;
  std::array<float, 5> values;

  for (auto& value : values) {
    std::getline(stream, line);
    std::stringstream line_stream{line};
    line_stream >> label >> value;
  }
  auto [mem_total, mem_free, mem_available, buffers, cached] = values;
  return (mem_total - mem_free - buffers - cached) / mem_total;
}

long LinuxParser::UpTime(const char* info_path) {
  std::ifstream stream;
  if (info_path)
    stream.open(info_path);
  else
    stream.open(kProcDirectory + kUptimeFilename);
  if (!stream.is_open()) return 0;
  std::string line;
  long uptime;

  std::getline(stream, line);
  std::stringstream line_stream{line};
  line_stream >> uptime;
  return uptime;
}

long LinuxParser::Jiffies(const char* info_path) {
  auto cpu_data = LinuxParser::RawCpuStat(info_path);
  // system jiffies = sum off all jiffies
  return std::accumulate(cpu_data.begin(), cpu_data.end(), 0);
}

std::array<long, 10> LinuxParser::RawCpuStat(const char* info_path) {
  std::array<long, 10> cpu_data;
  std::ifstream stream;
  if (info_path)
    stream.open(info_path);
  else
    stream.open(kProcDirectory + kStatFilename);
  if (!stream.is_open()) return cpu_data;
  std::string line, label;
  std::getline(stream, line);
  std::stringstream line_stream{line};
  line_stream >> label;
  for (auto& stat : cpu_data) {
    line_stream >> stat;
  }
  return cpu_data;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid [[maybe_unused]],
                                const char* info_path) {
  return 0;
}

long LinuxParser::ActiveJiffies(const char* info_path) {
  return Jiffies(info_path) - IdleJiffies(info_path);
}

long LinuxParser::IdleJiffies(const char* info_path) {
  // idle_all_time = idleTime + ioWait
  auto cpu_data = LinuxParser::RawCpuStat(info_path);
  return cpu_data[3] + cpu_data[4];
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization(const char* info_path) { return {}; }

int LinuxParser::TotalProcesses(const char* info_path) {
  auto path = (info_path == nullptr) ? (kProcDirectory + kStatFilename)
                                     : std::string{info_path};
  return ScanAndGet<int>(path, "processes");
}

int LinuxParser::RunningProcesses(const char* info_path) {
  auto path = (info_path == nullptr) ? (kProcDirectory + kStatFilename)
                                     : std::string{info_path};
  return ScanAndGet<int>(path, "procs_running");
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid [[maybe_unused]], const char* info_path) {
  return string();
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid [[maybe_unused]], const char* info_path) {
  return string();
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid [[maybe_unused]], const char* info_path) {
  return string();
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid [[maybe_unused]], const char* info_path) {
  return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid [[maybe_unused]], const char* info_path) {
  return 0;
}

template <typename T>
T LinuxParser::ScanAndGet(const std::string& info_path, const std::string& key,
                          int offset) {
  std::ifstream stream{info_path};
  if (!stream.is_open()) return T();
  std::string label;
  T value;
  for (std::string label; stream >> label;) {
    if (label == key) {
      for (int i = 0; i < offset; i++) {
        stream >> value;
      }
      return value;
    }
  }
  return T();
}