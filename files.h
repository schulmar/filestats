/*
 * files.h
 *
 *  Created on: 03.12.2013
 *      Author: martin
 */

#ifndef FILES_H_
#define FILES_H_

#include <string>
#include <map>

#include "filestatistics.h"

/**
 * @brief Holds and prints the statistics for a set of files
 */
struct Files {
  typedef intptr_t FDType;

  /**
   * @brief Log opening of a new file with the given path
   */
  void open(const std::string &fileName, FDType fileDescriptor);

  /**
   * @brief Log read from a file with the given descriptor
   */
  void read(FDType fileDescriptor, unsigned bytes, std::uint64_t ns);

  /**
   * @brief Log write to a file with the given descriptor
   */
  void write(FDType fileDescriptor, unsigned bytes, std::uint64_t ns);

  /**
   * @brief Log closing of file
   */
  void close(FDType fileDescriptor);

  typedef std::map<std::string, FileStatistics> NameToStatistic;

  /**
   * @return Iterator to the first file in this statistic
   */
  NameToStatistic::const_iterator begin()const;

  /**
   * @return Iterator to the last file in this statistic
   */
  NameToStatistic::const_iterator end()const;

private:
  /// allow the files function to create an instance of this class
  friend Files &files();

  /**
   * @brief Create a new instance of this class
   *
   * This must only be called in files()
   */
  Files();

  /**
   * @brief Print final statistics (called on programm end)
   */
  ~Files();

  /**
   * @brief Print the current statistics every x seconds
   */
  void printTimed();

  /**
   * @brief Add other Files statistics
   */
  Files& operator+=(const Files& rhs);

  /// maps registered filenames to their statistics
  NameToStatistic nameToStatistic;
  typedef std::map<FDType, NameToStatistic::iterator> FileDescriptorToFile;
  /// maps filedescriptors to the attached name/staticstic pair
  FileDescriptorToFile fileDescriptorToFile;
  /// the last time print was executed
  time_t lastTime;
};

/**
 * @brief Print the statistics for all files
 */
void printStatistics(const Files &files);

/**
 * @return A single instance of the Files class
 */
Files &files();

#endif /* FILES_H_ */
