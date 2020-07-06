#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <unistd.h>
#include <iostream>
#include <vector>
#include "linux_parser.h"

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  std::vector<long> GetStats();
  long GetNonIdle(std::vector<long>);
  long GetIdle(std::vector<long>);
  const int TOKEN_COUNT = 10;
};

#endif