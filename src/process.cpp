#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() const { return m_pid; }

float Process::CpuUtilization() const
{  
    long total_time{LinuxParser::ActiveJiffies(m_pid)};
    long start_time_seconds{UpTime()};
    return (static_cast<float>(total_time)/sysconf(_SC_CLK_TCK)) / static_cast<float>(start_time_seconds);
}

string Process::Command() const { return LinuxParser::Command(m_pid); }

string Process::Ram() const { return LinuxParser::Ram(m_pid); }

string Process::User() const { return LinuxParser::User(m_pid); }

long int Process::UpTime() const { return LinuxParser::UpTime(m_pid); }

bool Process::operator<(Process const& a) const { return CpuUtilization() < a.CpuUtilization(); }

bool Process::operator==(Process const& other) const {return Pid() == other.Pid();}