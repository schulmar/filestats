/**
 * @brief files.cpp
 * @date 03.12.2013
 */

#include "files.h"

#include <iostream>
#include <algorithm>
#include <iomanip>
#include <utility>

Files::Files()
: nameToStatistic(), fileDescriptorToFile(), lastTime(time(nullptr)) {}

void Files::open(const std::string &fileName, FDType fileDescriptor) {
  fileDescriptorToFile[fileDescriptor] =
      nameToStatistic.insert(std::make_pair(fileName, FileStatistics())).first;
}

void Files::read(FDType fileDescriptor, unsigned bytes, std::uint64_t ns) {
  auto iter = fileDescriptorToFile.find(fileDescriptor);
  if (iter != fileDescriptorToFile.end()) {
    iter->second->second.read.bytes += bytes;
    iter->second->second.read.nanoSeconds += ns;
    ++(iter->second->second.read.calls);
  }
}

void Files::write(FDType fileDescriptor, unsigned bytes, std::uint64_t ns) {
  auto iter = fileDescriptorToFile.find(fileDescriptor);
  if (iter != fileDescriptorToFile.end()) {
    iter->second->second.write.bytes += bytes;
    iter->second->second.write.nanoSeconds += ns;
    ++(iter->second->second.write.calls);
  }
}

void Files::close(FDType fileDescriptor) {
  fileDescriptorToFile.erase(fileDescriptor);
}

Files::NameToStatistic::const_iterator Files::begin() const {
  return nameToStatistic.begin();
}

Files::NameToStatistic::const_iterator Files::end() const {
  return nameToStatistic.end();
}

Files &Files::operator+=(const Files &rhs) {
  /// add old statistics and create
  for (const auto &statistic : rhs)
    nameToStatistic[statistic.first] += statistic.second;
  /// calculate the mapping for the new file descriptors
  for (const auto &descriptor : rhs.fileDescriptorToFile)
    if (fileDescriptorToFile.find(descriptor.first) ==
        fileDescriptorToFile.end())
      fileDescriptorToFile[descriptor.first] =
          nameToStatistic.find(descriptor.second->first);
  return *this;
}

void Files::resetStatistics() {
  for (auto &statistic : nameToStatistic)
    statistic.second = FileStatistics();
}

namespace std {
/**
 * @brief output the amount formatted onto the
 */
static std::ostream &operator<<(std::ostream &stream,
                                const FileStatistics::Amount &amount) {
  stream << amount.calls << ", " << std::setprecision(2)
         << megaBytesPerSecond(amount) << "MB/s";
  return stream;
}
}

static void
printSingleFileStatistic(std::ostream &stream,
                         Files::NameToStatistic::const_reference file) {
  stream << file.first << "\n\t read: " << file.second.read
         << "\n\twrite: " << file.second.write << "\n";
}

void printStatistics(std::ostream &stream, const Files &files) {
  if (files.begin() != files.end()) {
    stream.setf(std::ios::fixed, std::ios::floatfield);
    std::for_each(files.begin(), files.end(),
                  std::bind(&printSingleFileStatistic, std::ref(stream),
                            std::placeholders::_1));
    stream << std::endl;
  }
}
