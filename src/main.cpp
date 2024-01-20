#include "ncurses_display.h"
#include "system.h"
#include "linux_parser.h"
#include <iostream>

int main() {
  System system;
  // std::cout<<LinuxParser::ActiveJiffies(96332);
  // std::cout<<LinuxParser::UpTime(96332);
  Process process{540492};
  std::cout<<process.User();
  // std::cout<<LinuxParser::Ram(518110);
  // LinuxParser::CpuUtilization();
  NCursesDisplay::Display(system);
}