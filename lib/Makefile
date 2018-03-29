CSVMONKEY_MAJOR=$(shell grep CSVMONKEY_MAJOR ../csvmonkey.hpp | awk '{print $$3}')
CSVMONKEY_MINOR=$(shell grep CSVMONKEY_MINOR ../csvmonkey.hpp | awk '{print $$3}')
CSVMONKEY_PATCH=$(shell grep CSVMONKEY_PATCH ../csvmonkey.hpp | awk '{print $$3}')
CSVMONKEY_SONAME=$(shell grep CSVMONKEY_SONAME ../csvmonkey.hpp |awk '{print $$3}')

CXXFLAGS=-msse4.2 -fPIC -g -ggdb
LDFLAGS=-lc
OPTIMIZATION?=-O3
OBJ=libcsvmonkey.o

DYLIBSUFFIX=so
STLIBSUFFIX=a
LIBNAME=libcsvmonkey

DYLIB_MINOR_NAME=$(LIBNAME).$(DYLIBSUFFIX).$(CSVMONKEY_SONAME)
DYLIB_MAJOR_NAME=$(LIBNAME).$(DYLIBSUFFIX).$(CSVMONKEY_MAJOR)
DYLIBNAME=$(LIBNAME).$(DYLIBSUFFIX)
DYLIB_MAKE_CMD=$(CXX) -shared -Wl,-soname,$(DYLIB_MINOR_NAME) -o $(DYLIBNAME) $(LDFLAGS)
STLIBNAME=$(LIBNAME).$(STLIBSUFFIX)
STLIB_MAKE_CMD=ar rcs $(STLIBNAME)

PREFIX?=/usr/local
INCLUDE_PATH?=include
LIBRARY_PATH?=lib
INSTALL_INCLUDE_PATH=$(PREFIX)/$(INCLUDE_PATH)
INSTALL_LIBRARY_PATH=$(PREFIX)/$(LIBRARY_PATH)

opsys := $(shell sh -c 'uname -s 2>/dev/null || echo not')
ifeq ($(opsys),Darwin)
  DYLIBSUFFIX=dylib
  DYLIB_MINOR_NAME=$(LIBNAME).$(CSVMONKEY_SONAME).$(DYLIBSUFFIX)
  DYLIB_MAKE_CMD=$(CXX) -shared -Wl,-install_name,$(DYLIB_MINOR_NAME) -o $(DYLIBNAME) $(LDFLAGS)
endif

ifeq ($(uname_S),$(filter $(uname_S),SunOS OpenBSD))
  INSTALL?= cp -r
endif
INSTALL?= cp -a

all: $(DYLIBNAME) $(STLIBNAME)

# Generated with g++ -MM or gcc -MM
libcsvmonkey.o: libcsvmonkey.cpp ../csvmonkey.hpp csvmonkey.h
example.o: example.c csvmonkey.h

dynamic: $(DYLIBNAME)
static: $(STLIBNAME)

%.o : %.cpp
	$(CXX) $(OPTIMIZATION) -std=c++11 -Wall -c $(CXXFLAGS) $<

%.o : %.c
	$(CC) $(OPTIMIZATION) -std=c99 -Wall -c $(CFLAGS) $<

$(DYLIBNAME): $(OBJ)
	$(DYLIB_MAKE_CMD) $(OBJ)

$(STLIBNAME): $(OBJ)
	$(STLIB_MAKE_CMD) $(OBJ)

debug: OPTIMIZATION=-O0
debug: all

install: $(DYLIBNAME) $(STLIBNAME)
	mkdir -p $(INSTALL_INCLUDE_PATH) $(INSTALL_LIBRARY_PATH)
	$(INSTALL) csvmonkey.h $(INSTALL_INCLUDE_PATH)
	$(INSTALL) $(DYLIBNAME) $(INSTALL_LIBRARY_PATH)/$(DYLIB_MINOR_NAME)
	cd $(INSTALL_LIBRARY_PATH) && ln -sf $(DYLIB_MINOR_NAME) $(DYLIBNAME)
	$(INSTALL) $(STLIBNAME) $(INSTALL_LIBRARY_PATH)

example: example.o $(STLIBNAME)
	$(CC) -oexample -lstdc++ $< $(STLIBNAME)

clean:
	rm -f $(STLIBNAME) $(DYLIBNAME) $(OBJ) *.o
