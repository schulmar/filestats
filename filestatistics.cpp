/**
 * @file filestatistics.cpp
 * @date 17.03.2013
 */

#include "filestatistics.h"

#include <limits>

FileStatistics::Amount &FileStatistics::Amount::operator+=(const Amount &rhs) {
  calls += rhs.calls;
  bytes += rhs.bytes;
  nanoSeconds += rhs.nanoSeconds;
  return *this;
}

FileStatistics::FileStatistics() : read{ 0, 0, 0 }, write{ 0, 0, 0 } {}

FileStatistics &FileStatistics::operator+=(const FileStatistics &rhs) {
  read += rhs.read;
  write += rhs.write;
  return *this;
}

double megaBytesPerSecond(const FileStatistics::Amount &amount) {
  if (amount.nanoSeconds)
    return 1e3 * amount.bytes / amount.nanoSeconds;
  else if (amount.bytes)
    return std::numeric_limits<double>::infinity();
  else
    return 0;
}
