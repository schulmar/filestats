/*
 * files.cpp
 *
 *  Created on: 03.12.2013
 *      Author: martin
 */

#include "files.h"

#include <iostream>
#include <algorithm>
#include <iomanip>
#include <utility>

Files::Files()
: nameToStatistic(), fileDescriptorToFile(), lastTime(time(nullptr)) {}

Files::~Files() { printStatistics(); }

void Files::open(const std::string &fileName, FDType fileDescriptor) {
  fileDescriptorToFile[fileDescriptor] =
      nameToStatistic.insert(std::make_pair(fileName, FileStatistics())).first;
  printTimed();
}

void Files::read(FDType fileDescriptor, unsigned bytes, std::uint64_t ns) {
  auto iter = fileDescriptorToFile.find(fileDescriptor);
  if (iter != fileDescriptorToFile.end()) {
    iter->second->second.read.bytes += bytes;
    iter->second->second.read.nanoSeconds += ns;
    ++(iter->second->second.read.calls);
    printTimed();
  }
}

void Files::write(FDType fileDescriptor, unsigned bytes, std::uint64_t ns) {
  auto iter = fileDescriptorToFile.find(fileDescriptor);
  if (iter != fileDescriptorToFile.end()) {
    iter->second->second.write.bytes += bytes;
    iter->second->second.write.nanoSeconds += ns;
    ++(iter->second->second.write.calls);
    printTimed();
  }
}

void Files::close(FDType fileDescriptor) {
  fileDescriptorToFile.erase(fileDescriptor);
  printStatistics();
}

void Files::printTimed() {
  time_t current = time(nullptr);
  if (current - lastTime > 10) {
    lastTime = current;
    printStatistics();
  }
}

void Files::printStatistics() {
  if (!nameToStatistic.empty()) {
    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    std::for_each(nameToStatistic.begin(), nameToStatistic.end(),
                  [&, this](const NameToStatistic::reference file) {
      std::cout << file.first << "\n\t read: " << file.second.read.calls << ", "
                << std::setprecision(2) << megaBytesPerSecond(file.second.read)
                << "MB/s"
                << "\n\twrite: " << file.second.write.calls << ", "
                << std::setprecision(2) << megaBytesPerSecond(file.second.write)
                << "MB/s)\n";
    });
    std::cout << std::endl;
  }
}

Files &files() {
  static Files files;
  return files;
}
