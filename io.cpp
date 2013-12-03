/*
 * io.cpp
 *
 *  Created on: 03.12.2013
 *      Author: martin
 */

#include "io.h"

#include <dlfcn.h>
#include <glob.h>
#include <unistd.h>

#include "files.h"

extern "C" {

/// create a functionpointer real_##FNAME that points to the real FNAME function
#define GET_REAL(FNAME)                                                        \
  typedef decltype(&FNAME) FPTR;                                               \
  static FPTR real_##FNAME = nullptr;                                          \
  if (!real_##FNAME)                                                           \
    real_##FNAME = (FPTR)dlsym(RTLD_NEXT, #FNAME);

FILE *fopen(__const char *__restrict __filename,
            __const char *__restrict __modes) {
  GET_REAL(fopen);
  FILE *fd = real_fopen(__filename, __modes);
  if (fd)
    files().open(__filename, fileno(fd));
  return fd;
}

FILE *fopen64(__const char *__restrict __filename,
              __const char *__restrict __modes) {
  GET_REAL(fopen64);
  FILE *fd = real_fopen64(__filename, __modes);
  if (fd)
    files().open(__filename, fileno(fd));
  return fd;
}

size_t fread(void *__restrict __ptr, size_t __size, size_t __n,
             FILE *__restrict __stream) {
  GET_REAL(fread);
  timespec tsBefore, tsAfter;
  clock_gettime(CLOCK_MONOTONIC, &tsBefore);
  ssize_t res = real_fread(__ptr, __size, __n, __stream);
  clock_gettime(CLOCK_MONOTONIC, &tsAfter);
  std::uint64_t diffTime = 1e9 * (tsAfter.tv_sec - tsBefore.tv_sec) +
                           tsAfter.tv_nsec - tsBefore.tv_nsec;
  files().read(fileno(__stream), res * __size, diffTime);
  return res;
}

size_t fwrite(__const void *__restrict __ptr, size_t __size, size_t __n,
              FILE *__restrict __s) {
  GET_REAL(fwrite);
  timespec tsBefore, tsAfter;
  clock_gettime(CLOCK_MONOTONIC, &tsBefore);
  ssize_t res = real_fwrite(__ptr, __size, __n, __s);
  clock_gettime(CLOCK_MONOTONIC, &tsAfter);
  std::uint64_t diffTime = 1e9 * (tsAfter.tv_sec - tsBefore.tv_sec) +
                           tsAfter.tv_nsec - tsBefore.tv_nsec;
  files().write(fileno(__s), res * __size, diffTime);
  return res;
}

size_t fread_unlocked(void *__restrict __ptr, size_t __size, size_t __n,
                      FILE *__restrict __stream) {
  GET_REAL(fread_unlocked);
  timespec tsBefore, tsAfter;
  clock_gettime(CLOCK_MONOTONIC, &tsBefore);
  ssize_t res = real_fread_unlocked(__ptr, __size, __n, __stream);
  clock_gettime(CLOCK_MONOTONIC, &tsAfter);
  std::uint64_t diffTime = 1e9 * (tsAfter.tv_sec - tsBefore.tv_sec) +
                           tsAfter.tv_nsec - tsBefore.tv_nsec;
  files().read(fileno(__stream), res * __size, diffTime);
  return res;
}

size_t fwrite_unlocked(__const void *__restrict __ptr, size_t __size,
                       size_t __n, FILE *__restrict __s) {
  GET_REAL(fwrite_unlocked)
  timespec tsBefore, tsAfter;
  clock_gettime(CLOCK_MONOTONIC, &tsBefore);
  ssize_t res = real_fwrite_unlocked(__ptr, __size, __n, __s);
  clock_gettime(CLOCK_MONOTONIC, &tsAfter);
  std::uint64_t diffTime = 1e9 * (tsAfter.tv_sec - tsBefore.tv_sec) +
                           tsAfter.tv_nsec - tsBefore.tv_nsec;
  files().write(fileno(__s), res * __size, diffTime);
  return res;
}

int creat(__const char *__file, __mode_t __mode) {
  GET_REAL(creat);
  int fd = real_creat(__file, __mode);
  files().open(__file, fd);
  return fd;
}

int creat64(__const char *__file, __mode_t __mode) {
  GET_REAL(creat64);
  int fd = real_creat64(__file, __mode);
  files().open(__file, fd);
  return fd;
}

int open(__const char *__file, int __oflag, ...) {
  GET_REAL(open);
  int fd = real_open(__file, __oflag);
  if (fd != -1)
    files().open(__file, fd);
  return fd;
}

int open64(__const char *__file, int __oflag, ...) {
  GET_REAL(open64);
  int fd = real_open64(__file, __oflag);
  if (fd != -1)
    files().open(__file, fd);
  return fd;
}

int close(int __fd) {
  GET_REAL(close);
  int ret = real_close(__fd);
  files().close(__fd);
  return ret;
}

ssize_t read(int __fd, void *__buf, size_t __nbytes) {
  GET_REAL(read);
  timespec tsBefore, tsAfter;
  clock_gettime(CLOCK_MONOTONIC, &tsBefore);
  ssize_t res = real_read(__fd, __buf, __nbytes);
  clock_gettime(CLOCK_MONOTONIC, &tsAfter);
  std::uint64_t diffTime = 1e9 * (tsAfter.tv_sec - tsBefore.tv_sec) +
                           tsAfter.tv_nsec - tsBefore.tv_nsec;
  files().read(__fd, res, diffTime);
  return res;
}

ssize_t write(int __fd, __const void *__buf, size_t __n) {
  GET_REAL(write);
  timespec tsBefore, tsAfter;
  clock_gettime(CLOCK_MONOTONIC, &tsBefore);
  ssize_t res = real_write(__fd, __buf, __n);
  clock_gettime(CLOCK_MONOTONIC, &tsAfter);
  std::uint64_t diffTime = 1e9 * (tsAfter.tv_sec - tsBefore.tv_sec) +
                           tsAfter.tv_nsec - tsBefore.tv_nsec;
  files().write(__fd, res, diffTime);
  return res;
}
}
