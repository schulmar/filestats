/**
 * @file filestatistics.cpp
 * @date 17.03.2013
 */

#include "filestatistics.h"

#include <limits>

double megaBytesPerSecond(const FileStatistics::Amount &amount) {
  if (amount.nanoSeconds)
    return 1e3 * amount.bytes / amount.nanoSeconds;
  else if (amount.bytes)
    return std::numeric_limits<double>::infinity();
  else
    return 0;
}
