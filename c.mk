CC = gcc
CXX = g++
CSTD = -std=c17
CXXSTD = -std=c++17
WARN = -pedantic -Wall -Wconversion -Wsign-conversion -Wsign-compare
LDLIBS =

src := $(wildcard *.c)
bin_c = $(src:%.c=bin-c/%)
bin_cpp = $(src:%.c=bin-cpp/%)

all: $(bin_c) $(bin_cpp)

# --------- General rules -----------

bin-c/%: %.c
	$(CC) $(WARN) $(CSTD) $^ $(LDLIBS) -o $@

bin-cpp/%: %.c
	$(CXX) $(WARN) $(CXXSTD) $^ $(LDLIBS) -o $@

# --------- Directories -------------

bin-c:
	mkdir -p $@

bin-cpp:
	mkdir -p $@

$(bin_c): | bin-c

$(bin_cpp): | bin-cpp

# ----------------------------------

clean:
	$(RM) -r bin-c bin-cpp

.PHONY: all clean
