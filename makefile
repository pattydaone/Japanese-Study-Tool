os := $(shell uname -s)
flags :=
libs :=
compiler :=

ifeq ($(os), Linux)
	flags += $(shell wx-config --cppflags)
	libs += $(shell wx-config --libs)
	compiler += g++
endif

ifeq ($(os), Darwin)
	flags += $(shell wx-config --cppflags)
	libs += $(shell wx-config --libs)
:wa
	compiler += clang++
endif

all: build clean run
build:
	$(compiler) $(flags) -Wall -g -c main.cpp -o main.o
	$(compiler) -o bin/Debug/testing main.o $(libs)
clean:
	rm -f main.o
run:
	bin/Debug/testing
testbuild: test.cpp
	$(compiler) test.cpp -o a.out
	./a.out
