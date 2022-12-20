all : scube.r

.PHONY : all
CXX = g++
CFLAGS = -g -D DEBUG -std=c++11 -static-libstdc++ -lpthread

scube.r : scube.o
	$(CXX) -o scube.r scube.o $(CFLAGS)

scube.o : main.cpp
	$(CXX) -o scube.o -c main.cpp -D SR8 -D ROOM2 $(CFLAGS)

.PHONY: clean
clean:
	-$(RM) scube.r
	-$(RM) scube.o

