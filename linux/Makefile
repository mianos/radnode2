CXX=clang++
# CXX=clang++-8

CPPFLAGS=-ggdb -std=c++14 -Wall -I../src -I.

all: maq

maq: maq.o 
	$(CXX) -o maq maq.o

maq.o: ../src/maq.h ../src/squeue.h

maq.o: ../src/maq.cpp
	$(CXX) $(CPPFLAGS) -c  ../src/maq.cpp
