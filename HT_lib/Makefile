main : main.o HT.o
	gcc -o main HT.o main.o ../BF_lib/BF_64.a -no-pie -lstdc++ -w
main.o : HT.cpp HT.hpp
	gcc -std=c++11 -c main.cpp -w

HT.o : HT.cpp HT.hpp ../BF_lib/BF_64.a
	gcc -std=c++11 -c HT.cpp -w

clean : 
	rm main.o HT.o main

run : main
	./main
