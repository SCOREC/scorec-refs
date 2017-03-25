fixrefs : fixrefs.cpp
	$(CXX) -g -O0 -std=c++11 $< -o $@
