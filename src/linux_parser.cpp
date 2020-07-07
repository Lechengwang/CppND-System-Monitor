#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
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

float LinuxParser::MemoryUtilization() {
  string tag, kb;
  long total, free;
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> tag >> total >> kb;
    
    std::getline(stream, line);
    std::istringstream linestream1(line);
    linestream1 >> tag >> free >> kb;
  }
  return ((float)total - (float)free) / (float)total; 
}

long LinuxParser::UpTime() {
  double uptime, idletime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idletime;
  }  
  return (long) uptime;
}

long LinuxParser::Jiffies() { 
  // jiffies = uptime * freq
  long uptime = LinuxParser::UpTime();
  long freq = sysconf(_SC_CLK_TCK);
 return uptime * freq; 
}

long LinuxParser::ActiveJiffies(int pid) {
  // per pid jiffie is token 13-16 (starting from 0) of prod/pid/stat file
  int stat_token = 17;
  long result = 0;
  string token[stat_token];
  string line;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + "/" + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < stat_token; i ++) {
      linestream >> token[i];
      if (i >= 13 && i <= 16) {
        result += std::stoi(token[i]);
      }
    }
  }
  return result;
}

long LinuxParser::ActiveJiffies() {
  int stat_token = 10;
  long result = 0;
  long token[stat_token];
  string cpu;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    // We only care about the 1st line of the stat file for jiffies
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    for (int i = 0; i < stat_token; i ++) {
      linestream >> token[i];
        result += token[i];
    }
  }
  return result;
}

long LinuxParser::IdleJiffies() {
  int stat_token = 10;
  long result = 0;
  long token[stat_token];
  string cpu;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    // We only care about the 1st line of the stat file for jiffies
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    for (int i = 0; i < stat_token; i ++) {
      linestream >> token[i];
      // idel jiffies are idle+iowait
      if (i == 3 || i == 4) {
        result += token[i];
      }
    }
  }
  return result;
}

float LinuxParser::CpuUtilization(int pid) {
  long uptime = LinuxParser::UpTime();
  string token[22];
  string line;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + "/" + kStatFilename); 
  if (stream.is_open()) {
    // We only care about the 1st line of the stat file for jiffies
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; i ++) {
      linestream >> token[i];
    }
  }
  long freq = sysconf(_SC_CLK_TCK);
  long total_time = std::stol(token[13]) + std::stol(token[14]) + std::stol(token[15]) + std::stol(token[16]);
  double seconds = uptime - stol(token[21]) / freq;
  double result = (double)total_time / freq / seconds;
  return (float)result;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { return 0; }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { return 0; }

string LinuxParser::Command(int pid) {
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + "/" + kCmdlineFilename);
  string line;
  if(stream.is_open()) {
    std::getline(stream, line, '\r');
    return line;
  }
}

string LinuxParser::Ram(int pid) {
  string key, value;
  string line;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + "/" + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
     if (line.find("VmSize:") != std::string::npos) {
       std::istringstream linestream(line);
       linestream >> key >> value;
       return std::to_string(stoi(value) / 1024) + "MB";
     }
    }
  }}

string LinuxParser::Uid(int pid[[maybe_unused]]) { 
  string key, value;
  string line;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + "/" + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
     if (line.find("Uid:") != std::string::npos) {
       std::istringstream linestream(line);
       linestream >> key >> value;
       return value;
     }
    }
  }
}

string LinuxParser::User(int pid) {
  string uid = LinuxParser::Uid(pid);
  string user, x, id;
  string line;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
     std::replace(line.begin(), line.end(), ':', ' ');
     std::istringstream linestream(line);
     linestream >> user >> x >> id;
     if (id == uid) {
       return user;
     }
    }
  }
}

long LinuxParser::UpTime(int pid) { 
  string token[22];
  string line;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + "/" + kStatFilename); 
  if (stream.is_open()) {
    // We only care about the 1st line of the stat file for jiffies
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; i ++) {
      linestream >> token[i];
    }
  }
  long freq = sysconf(_SC_CLK_TCK);
  return std::stol(token[21]) / freq;
}