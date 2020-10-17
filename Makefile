CXX=g++
CXXFLAGS=-std=c++17 -I.
ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

thermal: thermal.o
	$(CXX) -o thermal thermal.o

install: thermal
	cp thermal $(PREFIX)/bin
