objects = main.o graph.o

main : $(objects)
	g++ $(objects) -o main

main.o : graph.h
	g++ -c main.cpp
graph.o : graph.h
	g++ -c graph.cpp
clean :
	rm main $(objects)
