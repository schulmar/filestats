/**
 * @file filestatistics.h
 * @date 03.12.2013
 */

#ifndef FILESTATISTICS_H_
#define FILESTATISTICS_H_

#include <cstdint>

/**
 * @brief Holds the read/write statistics for one file
 */
struct FileStatistics {
  /**
   * @brief Holds the information about operations in one direction (read ^
   *        write)
   */
  struct Amount {
    /// the number of times read/write was called
    unsigned long calls;
    /// the sum of bytes read/written
    unsigned long bytes;
    /// the sum of nanoSeconds to finish the calls
    std::uint64_t nanoSeconds;
  };

  /// statistics for reading and writing
  Amount read, write;

  /**
   * @brief Initialize all statistics to zero
   */
  FileStatistics() : read{ 0, 0, 0 }, write{ 0, 0, 0 } {}
};

/**
 * @brief Get the MB/s for an Amount
 */
double megaBytesPerSecond(const FileStatistics::Amount &amount);

#endif /* FILESTATISTICS_H_ */
