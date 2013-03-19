all: filestatistics.so

clean:
	rm filestatistics.so
filestatistics.so: filestatistics.cpp
	g++ -std=c++11 -shared -ldl -fPIC filestatistics.cpp -o filestatistics.so