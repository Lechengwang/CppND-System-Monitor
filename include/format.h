#ifndef FORMAT_H
#define FORMAT_H

#include <string>

namespace Format {
int const M2S = 60;
int const H2M = 60;
int const H2S = H2M * M2S;
std::string ElapsedTime(long times);  // TODO: See src/format.cpp
};                                    // namespace Format

#endif