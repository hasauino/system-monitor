#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <array>
#include <numeric>
#include <optional>
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
        if (key == kPrettyName) {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
    filestream.close();
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
  stream.close();
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids(const char* info_path) {
  vector<int> pids;
  DIR* directory;
  if (info_path)
    directory = opendir(info_path);
  else
    directory = opendir(kProcDirectory.c_str());

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
  stream.close();
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
  stream.close();
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
  stream.close();
  std::stringstream line_stream{line};
  line_stream >> label;
  for (auto& stat : cpu_data) {
    line_stream >> stat;
  }
  return cpu_data;
}

long LinuxParser::ActiveJiffies(int pid, const char* info_path) {
  std::ifstream file;
  std::string stat_file_path = std::to_string(pid) + kStatFilename;
  if (info_path)
    file.open(std::string{info_path} + stat_file_path);
  else
    file.open(kProcDirectory + stat_file_path);
  if (!file.is_open()) return 0;
  std::string dummy;
  for (int i = 0; i < 13; i++) file >> dummy;
  long utime, stime;
  file >> utime >> stime;
  file.close();
  return utime + stime;
}

long LinuxParser::ActiveJiffies(const char* info_path) {
  return Jiffies(info_path) - IdleJiffies(info_path);
}

long LinuxParser::IdleJiffies(const char* info_path) {
  // idle_all_time = idleTime + ioWait
  auto cpu_data = LinuxParser::RawCpuStat(info_path);
  return cpu_data[kIdle_] + cpu_data[kIOwait_];
}

int LinuxParser::TotalProcesses(const char* info_path) {
  auto path = (info_path == nullptr) ? (kProcDirectory + kStatFilename)
                                     : std::string{info_path};
  auto processes = ScanAndGet<int>(path, kProcesses);
  return (processes.has_value()) ? processes.value() : 0;
}

int LinuxParser::RunningProcesses(const char* info_path) {
  auto path = (info_path == nullptr) ? (kProcDirectory + kStatFilename)
                                     : std::string{info_path};
  auto processes = ScanAndGet<int>(path, kRunningProcesses);
  return (processes.has_value()) ? processes.value() : 0;
}

string LinuxParser::Command(int pid, const char* info_path) {
  std::ifstream file;
  std::string cmd_file_path = std::to_string(pid) + kCmdlineFilename;
  if (info_path)
    file.open(std::string{info_path} + cmd_file_path);
  else
    file.open(kProcDirectory + cmd_file_path);
  if (!file.is_open()) return string();
  std::string command;
  file >> command;
  file.close();
  return TrimText(command);
}

string LinuxParser::Ram(int pid [[maybe_unused]], const char* info_path) {
  std::string stat_file_path;
  if (info_path)
    stat_file_path = info_path;
  else
    stat_file_path = kProcDirectory;
  stat_file_path = stat_file_path + std::to_string(pid) + kStatusFilename;

  // Here I used VmRSS instead of VmSize to get usage of the
  // phyiscal memory only as suggested by the reviewer.
  // It actually made things better, as I was getting Ram values
  // higher than my physical memory when I used VmSize
  auto vm_size = ScanAndGet<int>(stat_file_path, kVmRSS);
  if (!vm_size.has_value()) return string();
  auto ram = vm_size.value() / 1024;
  return std::to_string(ram);
}

string LinuxParser::User(int pid, const char* proc_path,
                         const char* passwd_path) {
  std::string stat_file_path;
  if (proc_path)
    stat_file_path = proc_path;
  else
    stat_file_path = kProcDirectory;
  stat_file_path = stat_file_path + std::to_string(pid) + kStatusFilename;
  auto uid = ScanAndGet<int>(stat_file_path, kUid);
  if (!uid.has_value()) return string();
  std::ifstream passwd_file;
  if (passwd_path)
    passwd_file.open(passwd_path);
  else
    passwd_file.open(kPasswordPath);
  if (!passwd_file.is_open()) return string();
  std::string line;
  std::stringstream line_stream;
  std::string user_name, x;
  int user_id;
  while (std::getline(passwd_file, line)) {
    std::replace(line.begin(), line.end(), ':', ' ');
    line_stream.str(line);
    line_stream >> user_name >> x >> user_id;
    if (user_id == uid.value()) return user_name;
  }
  passwd_file.close();
  return string();
}

long LinuxParser::UpTime(int pid [[maybe_unused]], const char* stat_root,
                         const char* uptime_path) {
  std::ifstream file;
  std::string stat_file_path;
  if (stat_root)
    stat_file_path = stat_root;
  else
    stat_file_path = kProcDirectory;
  stat_file_path = stat_file_path + std::to_string(pid) + kStatFilename;
  file.open(stat_file_path);
  if (!file.is_open()) return 0;
  std::string value;
  for (int i = 0; i < 22; i++) {
    file >> value;
  }
  file.close();
  auto start_time = std::stof(value) / sysconf(_SC_CLK_TCK);
  return UpTime(uptime_path) - start_time;
}

template <typename T>
std::optional<T> LinuxParser::ScanAndGet(const std::string& info_path,
                                         const std::string& key, int offset) {
  std::ifstream stream{info_path};
  if (!stream.is_open()) return std::nullopt;
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
  stream.close();
  return std::nullopt;
}

std::string LinuxParser::TrimText(const std::string& text, std::size_t max_length) {
  if (text.size() <= max_length) return text;
  return text.substr(0, max_length) + "...";
}