/**
 * @file filestatistics.cpp
 * @date 17.03.2013
 */

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dlfcn.h>

#include <stdint.h>

#include <string>
#include <map>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <limits>

struct FileStatistics
{
	struct Amount{
		unsigned long calls;
		unsigned long bytes;
		uint64_t nanoSeconds;
	};

	Amount read, write;

	FileStatistics()
	: read{0, 0, 0}, write{0, 0, 0}
	{}
};

double megaBytesPerSecond(const FileStatistics::Amount &amount)
{
	if(amount.nanoSeconds)
		return 1e3 * amount.bytes / amount.nanoSeconds;
	else
		if(amount.bytes)
			return std::numeric_limits<double>::infinity();
		else
			return 0;
}

struct Files
{
	typedef intptr_t FDType;

	Files()
	: nameToStatistic(),
	  fileDescriptorToFile(),
	  lastTime(time(nullptr))
	{}

	~Files()
	{
		printStatistics();
	}

	void open(const std::string &fileName, FDType fileDescriptor)
	{
		fileDescriptorToFile[fileDescriptor] = nameToStatistic.insert(
						std::make_pair(fileName, FileStatistics())).first;
		printTimed();
	}

	void read(FDType fileDescriptor, unsigned bytes, uint64_t ns)
	{
		auto iter = fileDescriptorToFile.find(fileDescriptor);
		if(iter != fileDescriptorToFile.end())
		{
			iter->second->second.read.bytes += bytes;
			iter->second->second.read.nanoSeconds += ns;
			++(iter->second->second.read.calls);
			printTimed();
		}
	}

	void write(FDType fileDescriptor, unsigned bytes, uint64_t ns)
	{
		auto iter = fileDescriptorToFile.find(fileDescriptor);
		if(iter != fileDescriptorToFile.end())
		{
			iter->second->second.write.bytes += bytes;
			iter->second->second.write.nanoSeconds += ns;
			++(iter->second->second.write.calls);
			printTimed();
		}
	}

	void close(FDType fileDescriptor)
	{
		fileDescriptorToFile.erase(fileDescriptor);
		printStatistics();
	}
private:
	void printTimed()
	{
		time_t current = time(nullptr);
		if(current - lastTime > 10)
		{
			lastTime = current;
			printStatistics();
		}
	}

	void printStatistics()
	{
		if(!nameToStatistic.empty())
		{
			std::cout.setf(std::ios::fixed, std::ios::floatfield);
			std::for_each(nameToStatistic.begin(), nameToStatistic.end(),
			[&, this](const NameToStatistic::reference file){
				std::cout << file.first
							<< "\n\t read: "
							<< file.second.read.calls << ", "
							<< std::setprecision(2)
							<< megaBytesPerSecond(file.second.read)
							<< "MB/s"
							<< "\n\twrite: "
							<< file.second.write.calls << ", "
							<< std::setprecision(2)
							<< megaBytesPerSecond(file.second.write)
							<< "MB/s)\n";
			});
			std::cout << std::endl;
		}
	}

	typedef std::map<std::string, FileStatistics> NameToStatistic;
	NameToStatistic nameToStatistic;
	typedef std::map<FDType, NameToStatistic::iterator> FileDescriptorToFile;
	FileDescriptorToFile fileDescriptorToFile;
	time_t lastTime;
};

Files &files()
{
	static Files files;
	return files;
}

extern "C"
{

FILE *fopen (__const char *__restrict __filename,
		    __const char *__restrict __modes)
{
	static FILE * (*real_fopen) (__const char *__restrict __filename,
		    __const char *__restrict __modes) = nullptr;
	if(!real_fopen)
		real_fopen = (FILE *(*)(__const char *__restrict __filename,
			    __const char *__restrict __modes)) dlsym(RTLD_NEXT, "fopen");
	FILE* fd = real_fopen(__filename, __modes);
	if(fd)
		files().open(__filename, fileno(fd));
	return fd;
}

FILE *fopen64 (__const char *__restrict __filename,
		      __const char *__restrict __modes)
{
	static FILE * (*real_fopen64) (__const char *__restrict __filename,
		    __const char *__restrict __modes) = nullptr;
	if(!real_fopen64)
		real_fopen64 = (FILE *(*)(__const char *__restrict __filename,
			    __const char *__restrict __modes)) dlsym(RTLD_NEXT, "fopen64");
	FILE* fd = real_fopen64(__filename, __modes);
	if(fd)
		files().open(__filename, fileno(fd));
	return fd;
}

size_t fread (void *__restrict __ptr, size_t __size,
		     size_t __n, FILE *__restrict __stream)
{
	static size_t (*real_fread) (void *__restrict, size_t, size_t, FILE *__restrict) = nullptr;
	if(!real_fread)
		real_fread = (size_t(*)(void *__restrict, size_t, size_t, FILE *__restrict))
					dlsym(RTLD_NEXT, "fread");
	timespec tsBefore, tsAfter;
	clock_gettime(CLOCK_MONOTONIC, &tsBefore);
	ssize_t res = real_fread(__ptr, __size, __n, __stream);
	clock_gettime(CLOCK_MONOTONIC, &tsAfter);
	uint64_t diffTime = 1e9 * (tsAfter.tv_sec - tsBefore.tv_sec)
						+ tsAfter.tv_nsec - tsBefore.tv_nsec;
	files().read(fileno(__stream), res * __size, diffTime);
	return res;
}

size_t fwrite (__const void *__restrict __ptr, size_t __size,
		      size_t __n, FILE *__restrict __s)
{
	static size_t (*real_fwrite) (__const void *__restrict, size_t, size_t, FILE *__restrict) = nullptr;
		if(!real_fwrite)
			real_fwrite = (size_t(*)(__const void *__restrict, size_t, size_t, FILE *__restrict))
						dlsym(RTLD_NEXT, "fwrite");
	timespec tsBefore, tsAfter;
	clock_gettime(CLOCK_MONOTONIC, &tsBefore);
	ssize_t res = real_fwrite(__ptr, __size, __n, __s);
	clock_gettime(CLOCK_MONOTONIC, &tsAfter);
	uint64_t diffTime = 1e9 * (tsAfter.tv_sec - tsBefore.tv_sec)
						+ tsAfter.tv_nsec - tsBefore.tv_nsec;
	files().write(fileno(__s), res * __size, diffTime);
	return res;
}

size_t fread_unlocked (void *__restrict __ptr, size_t __size,
			      size_t __n, FILE *__restrict __stream)
{
	static size_t (*real_fread_unlocked) (void *__restrict, size_t, size_t, FILE *__restrict) = nullptr;
	if(!real_fread_unlocked)
		real_fread_unlocked = (size_t(*)(void *__restrict, size_t, size_t, FILE *__restrict))
					dlsym(RTLD_NEXT, "fread_unlocked");
	timespec tsBefore, tsAfter;
	clock_gettime(CLOCK_MONOTONIC, &tsBefore);
	ssize_t res = real_fread_unlocked(__ptr, __size, __n, __stream);
	clock_gettime(CLOCK_MONOTONIC, &tsAfter);
	uint64_t diffTime = 1e9 * (tsAfter.tv_sec - tsBefore.tv_sec)
						+ tsAfter.tv_nsec - tsBefore.tv_nsec;
	files().read(fileno(__stream), res * __size, diffTime);
	return res;
}

size_t fwrite_unlocked (__const void *__restrict __ptr, size_t __size,
			       size_t __n, FILE *__restrict __s)
{
	static size_t (*real_fwrite_unlocked) (__const void *__restrict, size_t, size_t, FILE *__restrict) = nullptr;
		if(!real_fwrite_unlocked)
			real_fwrite_unlocked = (size_t(*)(__const void *__restrict, size_t, size_t, FILE *__restrict))
						dlsym(RTLD_NEXT, "fwrite_unlocked");
	timespec tsBefore, tsAfter;
	clock_gettime(CLOCK_MONOTONIC, &tsBefore);
	ssize_t res = real_fwrite_unlocked(__ptr, __size, __n, __s);
	clock_gettime(CLOCK_MONOTONIC, &tsAfter);
	uint64_t diffTime = 1e9 * (tsAfter.tv_sec - tsBefore.tv_sec)
						+ tsAfter.tv_nsec - tsBefore.tv_nsec;
	files().write(fileno(__s), res * __size, diffTime);
	return res;
}

int creat (__const char *__file, __mode_t __mode)
{
	static int (*real_creat) (__const char *, __mode_t) = nullptr;
	if(!real_creat)
		real_creat = (int(*)(__const char *, __mode_t)) dlsym(RTLD_NEXT, "creat");
	int fd = real_creat(__file, __mode);
	files().open(__file, fd);
	return fd;
}

int creat64 (__const char *__file, __mode_t __mode)
{
	static int (*real_creat64) (__const char *, __mode_t) = nullptr;
	if(!real_creat64)
		real_creat64 = (int(*)(__const char *, __mode_t)) dlsym(RTLD_NEXT, "creat64");
	int fd = real_creat64(__file, __mode);
	files().open(__file, fd);
	return fd;
}

int open (__const char *__file, int __oflag, ...)
{
	static int (*real_open) (__const char *, int, ...) = nullptr;
	if(!real_open)
		real_open = (int(*)(__const char *, int, ...)) dlsym(RTLD_NEXT, "open");
	int fd = real_open(__file, __oflag);
	files().open(__file, fd);
	return fd;
}

int open64 (__const char *__file, int __oflag, ...)
{
	static int (*real_open64) (__const char *, int, ...) = nullptr;
	if(!real_open64)
		real_open64 = (int(*)(__const char *, int, ...)) dlsym(RTLD_NEXT, "open64");
	int fd = real_open64(__file, __oflag);
	files().open(__file, fd);
	return fd;
}

int close (int __fd)
{
	static int (*real_close)(int) = nullptr;
	if(!real_close)
		real_close = (int (*)(int)) dlsym(RTLD_NEXT, "close");
	int ret = real_close(__fd);
	files().close(__fd);
	return ret;
}

ssize_t read (int __fd, void *__buf, size_t __nbytes)
{
	static ssize_t (*real_read) (int, void *, size_t) = nullptr;
	if(!real_read)
		real_read = (ssize_t(*) (int, void *, size_t))
					dlsym(RTLD_NEXT, "read");
	timespec tsBefore, tsAfter;
	clock_gettime(CLOCK_MONOTONIC, &tsBefore);
	ssize_t res = real_read(__fd, __buf, __nbytes);
	clock_gettime(CLOCK_MONOTONIC, &tsAfter);
	uint64_t diffTime = 1e9 * (tsAfter.tv_sec - tsBefore.tv_sec)
						+ tsAfter.tv_nsec - tsBefore.tv_nsec;
	files().read(__fd, res, diffTime);
	return res;
}

ssize_t write (int __fd, __const void *__buf, size_t __n)
{
	static ssize_t (*real_write) (int, __const void *, size_t) = nullptr;
		if(!real_write)
			real_write = (ssize_t (*) (int, __const void *, size_t))
						dlsym(RTLD_NEXT, "write");
	timespec tsBefore, tsAfter;
	clock_gettime(CLOCK_MONOTONIC, &tsBefore);
	ssize_t res = real_write(__fd, __buf, __n);
	clock_gettime(CLOCK_MONOTONIC, &tsAfter);
	uint64_t diffTime = 1e9 * (tsAfter.tv_sec - tsBefore.tv_sec)
						+ tsAfter.tv_nsec - tsBefore.tv_nsec;
	files().write(__fd, res, diffTime);
	return res;
}

}
