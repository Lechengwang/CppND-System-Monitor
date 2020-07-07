#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return pid_; }

float Process::CpuUtilization() {
  return LinuxParser::CpuUtilization(pid_);
}

string Process::Command() {
  return LinuxParser::Command(pid_);
}

// TODO: Return this process's memory utilization
string Process::Ram() { return string(); }

string Process::User() {
  return LinuxParser::User(pid_); 
}

long int Process::UpTime() {
  return LinuxParser::UpTime(pid_);
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }