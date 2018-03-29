
CXXFLAGS+=-std=c++11
CXXFLAGS+=-O3
CXXFLAGS+=-Wall
CXXFLAGS+=-lc

debug: CXXFLAGS+=-O0 -g
debug: test

release: X=-DNDEBUG
release: test

test: test.cpp csvmonkey.hpp Makefile
	g++ -std=c++11 $(CXXFLAGS) -msse4.2 $(X) -g -o test test.cpp

clean:
	rm -f test cachegrind* perf.data* *.gcda
	rm -f lib/*.o *.dylib *.so

pgo: X+=-DNDEBUG
pgo:
	g++ -std=c++11 $(CXXFLAGS) -DNDEBUG -fprofile-generate -msse4.2 $(X) -g -o test test.cpp
	./test profiledata.csv
	g++ -std=c++11 $(CXXFLAGS) -DNDEBUG -fprofile-use -msse4.2 $(X) -g -o test test.cpp

grind:
	rm -f cachegrind.out.*
	valgrind --tool=cachegrind --branch-sim=yes ./test ram.64mb.csv
	cg_annotate --auto=yes cachegrind.out.*

library:
	cd lib && make
