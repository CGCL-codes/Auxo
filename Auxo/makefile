objects = main.o Auxo.o AuxoPro.o GSS_Chain.o hashFunction.o

main : $(objects)
	g++ $(objects) -o main

main.o : querysupportstruct.h
	g++ -c main.cpp
Auxo.o : Auxo.cpp querysupportstruct.h hashFunction.o
	g++ -c Auxo.cpp
ComAuxo.o : AuxoPro.cpp querysupportstruct.h hashFunction.o
	g++ -c AuxoPro.cpp
GSS_Chain.o : GSS_Chain.cpp querysupportstruct.h hashFunction.o
	g++ -c GSS_Chain.cpp
hashFunction.o : hashFunction.cpp querysupportstruct.h
	g++ -c hashFunction.cpp
clean :
	rm main $(objects)
