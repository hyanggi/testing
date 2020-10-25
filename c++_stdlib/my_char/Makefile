CXX = g++
CXXSTD = -std=c++17

ifdef chkhdr
WARN = -Wsystem-headers
else
WARN = -pedantic -Wall -Wconversion -Wsign-conversion -Wsign-compare
endif

src := $(wildcard *.cpp)
bin = $(src:%.cpp=bin/%)

all: $(bin)

# --------- General rule -----------

bin/%: %.cpp *.h
	$(CXX) $(WARN) $(CXXSTD) $< -o $@

# --------- Directory --------------

bin:
	mkdir -p $@

$(bin): | bin

# ----------------------------------

clean:
	$(RM) -r bin

.PHONY: all clean
