CC = gcc
CFLAGS = -std=c17 -pthread -pedantic -Wall -Wconversion -Wsign-conversion -Wsign-compare
CXX = g++
CXXFLAGS = -std=c++2a -pthread -pedantic -Wall -Wconversion -Wsign-conversion -Wsign-compare

all: cmonitor cpxmonitor cppmonitor

cmonitor: monitor.c
	$(CC) $(CFLAGS) -o $@ $^

cpxmonitor: monitor.posix.c
	$(CC) $(CFLAGS) -o $@ $^

cppmonitor: monitor.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

test: all
	./test ./cmonitor ./cpxmonitor ./cppmonitor

clean:
	$(RM) cmonitor cpxmonitor cppmonitor

.PHONY: all test clean
