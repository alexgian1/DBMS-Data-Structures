main : main.o HT.o
	g++ ../BF_lib/BF_64.a HT.o main.o -no-pie -o main


main.o : HT.cpp HT.hpp
	g++ -c main.cpp

HT.o : HT.cpp HT.hpp ../BF_lib/BF_64.a
	g++ -c HT.cpp

clean : 
	rm main.o HT.o 

run : main
	./main
