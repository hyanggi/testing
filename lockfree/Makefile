CC = gcc
CFLAGS = -std=c17 -pthread -pedantic -Wall -Wconversion -Wsign-conversion -Wsign-compare
CXX = g++
CXXFLAGS = -std=c++2a -pthread -pedantic -Wall -Wconversion -Wsign-conversion -Wsign-compare

all: cringbuffer cppringbuffer

cringbuffer: ringbuffer.c
	$(CC) $(CFLAGS) -o $@ $^

cppringbuffer: ringbuffer.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	$(RM) cringbuffer cppringbuffer

.PHONY: all clean
