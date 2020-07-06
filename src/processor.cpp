#include "processor.h"

using namespace LinuxParser;
float Processor::Utilization() {
  std::vector<long> prevStat = Processor::GetStats();
  long prevNonIdle = Processor::GetNonIdle(prevStat);
  long prevIdle = Processor::GetIdle(prevStat);
  
  sleep(1);
  std::vector<long> stat = Processor::GetStats();
  long nonIdle = Processor::GetNonIdle(stat);
  long idle = Processor::GetIdle(stat);
  
  double totalD = nonIdle + idle - prevNonIdle - prevIdle;
  double idleD = idle - prevIdle;
  return (totalD - idleD) / totalD;
}

std::vector<long> Processor::GetStats() {
  std::vector<long> token(TOKEN_COUNT);
  std::string cpu;
  std::string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    // We only care about the 1st line of the stat file for CPU utilization
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    for (int i = 0; i < TOKEN_COUNT; i ++) {
      linestream >> token[i];
      }
    }
  return token;
}

long Processor::GetNonIdle(std::vector<long> token) {
  return token[0] + token[1] + token[2] + token[5] + token[6] + token [7];
}

long Processor::GetIdle(std::vector<long> token) {
  return token[3] + token[4];
}