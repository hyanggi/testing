CXX = g++
CXXSTD = -std=c++17
WARN = -pedantic -Wall -Wconversion -Wsign-conversion -Wsign-compare

src := $(wildcard *.cpp)
bin = $(src:%.cpp=bin/%)

all: $(bin)

# --------- General rule -----------

bin/%: %.cpp
	$(CXX) $(WARN) $(CXXSTD) $^ -o $@

# --------- Directory --------------

bin:
	mkdir -p $@

$(bin): | bin

# ----------------------------------

clean:
	$(RM) -r bin

.PHONY: all clean
