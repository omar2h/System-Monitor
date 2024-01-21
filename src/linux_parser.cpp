#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
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

string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
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

// https://stackoverflow.com/questions/41224738/how-to-calculate-system-memory-usage-from-proc-meminfo-like-htop/41251290#41251290
float LinuxParser::MemoryUtilization()
{
  float memory_utilization{};
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if(stream.is_open()) {
    string token{};
    unsigned long memTotal{};
    unsigned long cached{};
    unsigned long SReclaimable{};
    unsigned long Shmem{};
    unsigned long MemFree{};
    unsigned long Buffers{};
    while(stream >> token) {
      if(token == "MemTotal:") {
        stream >> token;
        memTotal = stol(token);
      }
      else if(token == "Cached:") {
        stream >> token;
        cached = stol(token);
      }
      else if(token == "Shmem:") {
        stream >> token;
        Shmem = stol(token);
      }
      else if(token == "MemFree:") {
        stream >> token;
        MemFree = stol(token);
      }
      else if(token == "Buffers:") {
        stream >> token;
        Buffers = stol(token);
      }
      else if(token == "SReclaimable:") {
        stream >> token;
        SReclaimable = stol(token);
        break;
      }
    }
    unsigned long usedMem{memTotal - MemFree};
    unsigned long memCached{cached + SReclaimable - Shmem};
    unsigned long nonCachedMem{usedMem - (Buffers + memCached)};
    memory_utilization = static_cast<float>(memCached + nonCachedMem + Buffers)/static_cast<float>(memTotal);
  }
  return memory_utilization;
}

long LinuxParser::UpTime()
{ 
  long uptime{};
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if(stream.is_open()) {
    stream >> uptime;
  }
  return uptime;
}

long LinuxParser::Jiffies() { return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies(); }

long LinuxParser::ActiveJiffies(int pid)
{
  long jeffies{};
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(stream.is_open()) {
    string token{};
    // utime
    long user_time{};
    // stime
    long kernel_time{};
    // cutime
    long children_user_time{};
    // cstime
    long children_kernel_time{};

    for(size_t i{}; i<13; i++) {
      stream >> token;
    }
    stream >> user_time >> kernel_time >> children_user_time >> children_kernel_time;
    jeffies = user_time + kernel_time + children_user_time + children_kernel_time;
  }
  return jeffies;
}

long LinuxParser::ActiveJiffies()
{ 
  vector<string> active_jeffies = CpuUtilization();
  return stol(active_jeffies.at(kUser_)) + stol(active_jeffies.at(kNice_)) +
        stol(active_jeffies.at(kSystem_)) + stol(active_jeffies.at(kIRQ_)) +
        stol(active_jeffies.at(kSoftIRQ_)) + stol(active_jeffies.at(kSteal_)) +
        stol(active_jeffies.at(kGuest_)) + stol(active_jeffies.at(kGuestNice_));
}

long LinuxParser::IdleJiffies()
{ 
  vector<string> idle_jeffies = CpuUtilization();
  return stol(idle_jeffies.at(CPUStates::kIdle_)) + stol(idle_jeffies.at(CPUStates::kIOwait_));
}

vector<string> LinuxParser::CpuUtilization() 
{ 
   vector<string> values{};
   std::ifstream stream(kProcDirectory + kStatFilename);
   if(stream.is_open()) {
    string line{};
    string token{};
    std::getline(stream, line);
    std::istringstream linestream(line);
    if(linestream >> token) {
      if(token == "cpu") {
        while(linestream >> token) {
          values.emplace_back(token);
        }
      }
    }
   }
   return values;
}

int LinuxParser::TotalProcesses()
{ 
  int total_processes{};
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()) {
    string token{};
    while(stream >> token) {
      if(token == "processes") {
        stream >> token;
        total_processes = stoi(token);
      }
    }
  }
  return total_processes;
}

int LinuxParser::RunningProcesses()
{
  int running_processes{};
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()) {
    string token{};
    while(stream >> token) {
      if(token == "procs_running") {
        stream >> token;
        running_processes = stoi(token);
      }
    }
  }
  return running_processes;
}

string LinuxParser::Command(int pid) {
  string command{};
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, command);
  }
  return command; 
}

string LinuxParser::Ram(int pid)
{
  string ram{}; 
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    string token{};
    while(stream >> token) {
      if(token == "VmSize:") {
        stream >> ram;
        ram = std::to_string(stol(ram)/1024);
        break;
      }
    }
  }
  return ram;
}

string LinuxParser::Uid(int pid)
{ 
  string uid{}; 
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    string token{};
    while(stream >> token) {
      if(token == "Uid:") {
        stream >> uid;
        break;
      }
    }
  }
  return uid;
}

string LinuxParser::User(int pid)
{ 
  string username{};
  std::ifstream stream(LinuxParser::kPasswordPath);
  if (stream.is_open()) {
    string line{};
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      string uid{};
      string x{};
      std::getline(linestream, username, ':');
      std::getline(linestream, x, ':');
      std::getline(linestream, uid, ':');

      if(uid == Uid(pid))
        break;
    }
  }
  return username;
}

long LinuxParser::UpTime(int pid) 
{ 
  long seconds{};
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(stream.is_open()) {
    string token{};
    long start_time{};
    for(size_t i{}; i<21; i++) {
      stream >> token;
    }
    stream >> start_time;
    seconds = UpTime() -  (start_time / sysconf(_SC_CLK_TCK)); 
  }
  return seconds;
}
