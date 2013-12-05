filestats
=========

A shared library to monitor a process' file operations using the LD_PRELOAD trick.

Usage
=====

LD_PRELOAD=/path/to/filestatistics.so your_program

You can specify the outputfile of the statistics by setting FILESTATISTICS_LOG in the environment.
If you do not specify it, the value defaults to filestatistics.log