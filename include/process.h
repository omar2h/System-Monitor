#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid) : m_pid{pid} {}
  int Pid() const;                               
  std::string User() const;                    
  std::string Command() const;                   
  float CpuUtilization() const;                
  std::string Ram() const;                     
  long int UpTime() const;                     
  bool operator<(Process const& a) const;
  bool operator==(Process const& other) const;

 private:
    int m_pid{};
};

#endif