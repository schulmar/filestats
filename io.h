/**
 * @file io.h
 * @date 03.12.2013
 * @brief Forwarddeclarations of all overriden io functions
 */

#ifndef IO_H_
#define IO_H_

#include <stdio.h>

extern "C" {

FILE *fopen(__const char *__restrict __filename,
            __const char *__restrict __modes);

FILE *fopen64(__const char *__restrict __filename,
              __const char *__restrict __modes);

size_t fread(void *__restrict __ptr, size_t __size, size_t __n,
             FILE *__restrict __stream);

size_t fwrite(__const void *__restrict __ptr, size_t __size, size_t __n,
              FILE *__restrict __s);

size_t fread_unlocked(void *__restrict __ptr, size_t __size, size_t __n,
                      FILE *__restrict __stream);

size_t fwrite_unlocked(__const void *__restrict __ptr, size_t __size,
                       size_t __n, FILE *__restrict __s);

int creat(__const char *__file, __mode_t __mode);

int creat64(__const char *__file, __mode_t __mode);

int open(__const char *__file, int __oflag, ...);

int open64(__const char *__file, int __oflag, ...);

int close(int __fd);

ssize_t read(int __fd, void *__buf, size_t __nbytes);

ssize_t write(int __fd, __const void *__buf, size_t __n);
}

#endif /* IO_H_ */
