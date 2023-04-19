#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <optional>
#include <regex>
#include <string>

namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};
const std::string kVmRSS{"VmRSS:"};
const std::string kUid{"Uid:"};
const std::string kProcesses{"processes"};
const std::string kRunningProcesses{"procs_running"};
const std::string kPrettyName{"PRETTY_NAME"};
const std::size_t kStringTrimThreshold{40};

// System
float MemoryUtilization(const char* info_path = nullptr);
long UpTime(const char* info_path = nullptr);
std::vector<int> Pids(const char* info_path = nullptr);
int TotalProcesses(const char* info_path = nullptr);
int RunningProcesses(const char* info_path = nullptr);
std::string OperatingSystem(const char* info_path = nullptr);
std::string Kernel(const char* info_path = nullptr);

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};
long Jiffies(const char* info_path = nullptr);
long ActiveJiffies(const char* info_path = nullptr);
long ActiveJiffies(int pid, const char* info_path = nullptr);
long IdleJiffies(const char* info_path = nullptr);

// Processes
std::string Command(int pid, const char* info_path = nullptr);
std::string Ram(int pid, const char* info_path = nullptr);
std::string User(int pid, const char* proc_path = nullptr,
                 const char* passwd_path = nullptr);
long int UpTime(int pid, const char* info_path = nullptr,
                const char* uptime_path = nullptr);

// Helpers
std::array<long, 10> RawCpuStat(const char* info_path = nullptr);
template <typename T>
std::optional<T> ScanAndGet(const std::string& info_path,
                            const std::string& key, int offset = 1);
std::string TrimText(const std::string& text,
                     std::size_t max_length = kStringTrimThreshold);
};  // namespace LinuxParser

#endif