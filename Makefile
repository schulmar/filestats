all: filestatistics.so

clean:
	rm filestatistics.so
filestatistics.so: Makefile filestatistics.cpp
	g++ -std=c++11 -g -shared -ldl -fPIC filestatistics.cpp -o filestatistics.so