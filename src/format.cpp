#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
  long hh = seconds / Format::H2S;
  long reminder = seconds - Format::H2S * hh;
  long mm = reminder / Format::M2S;
  long ss = reminder - Format::M2S * mm;
  return std::to_string(hh) + ":" + std::to_string(mm) + ":" + std::to_string(ss);
}