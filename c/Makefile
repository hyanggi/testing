include ../c.mk

bin-cpp/print_any: print_any.c
	$(CXX_NOT_COMPATIBLE)

bin-cpp/array: array.c
	$(CXX_NOT_COMPATIBLE)

bin-c/restrict: restrict.c
	$(CC) $(WARN) $(CSTD) -O $^ -o $@
bin-cpp/restrict: restrict.c
	$(CXX_NOT_COMPATIBLE)

bin-cpp/num_struct: CXXSTD = -std=c++2a

CXX_NOT_COMPATIBLE = echo "echo '$^ is not compatible with C++.'" > $@ && chmod +x $@
