/*
 * getlogfilename.cpp
 *
 *  Created on: 05.12.2013
 *      Author: martin
 */

#include "getlogfilename.h"
#include <cstdlib>

static const char * envLogFileName = "FILESTATISTICS_LOG";

std::string getLogFileName() {
  const char *name = std::getenv(envLogFileName);
  if(!name)
    name = "filestatistics.log";
  return name;
}
