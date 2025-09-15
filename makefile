os := $(shell uname -s)
debugFlags :=
releaseFlags :=
debugLibs := 
releaseLibs :=
compiler :=

ifeq ($(os), Linux)
	debugFlags += $(shell wx-config --cxxflags)
	debugLibs  += $(shell wx-config --libs)
	compiler   += g++
endif

ifeq ($(os), Darwin)
	releaseFlags += $(shell ~/Builds/wxWidgetsRELEASE/wx-config --cxxflags)
	releaseLibs  += $(shell ~/Builds/wxWidgetsRELEASE/wx-config --libs)

	# debugFlags   += -DwxDEBUG_LEVEL=0
	debugFlags   += $(shell ~/Builds/wxWidgets/wx-config --cxxflags)
	debugLibs    += $(shell ~/Builds/wxWidgets/wx-config --libs)

	compiler     += clang++
endif

all: build clean run
debug:
	$(compiler) $(debugFlags) -Wc++17-extensions -Wall -g -O0 -c main.cpp -o main.o
	$(compiler) -o bin/Debug/testing main.o $(debugLibs)
build:
	$(compiler) $(releaseFlags) -Wc++17-extensions -Wall -g -O1 -c main.cpp -o main.o
	$(compiler) -o bin/Debug/testing main.o $(releaseLibs)
clean:
	rm -f main.o
run:
	bin/Debug/testing
testbuild:
	$(compiler) $(debugFlags) -Wc++17-extensions -Wall -g -O0 -c test.cpp -o a.o
	$(compiler) -o bin/Debug/testcpp a.o $(debugLibs)
	rm -f a.o
testrun:
	bin/Debug/testcpp
