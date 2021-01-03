main : SHT.o HT.o main.o 
	gcc -o main SHT.o main.o HT.o ../BF_lib/BF_64.a -no-pie -lstdc++ -w

SHT.o : SHT.cpp SHT.hpp ../BF_lib/BF_64.a HT.o
	gcc -std=c++11 -c SHT.cpp -w

HT.o : ../HT_lib/HT.cpp ../HT_lib/HT.hpp ../BF_lib/BF_64.a
	gcc -std=c++11 -c ../HT_lib/HT.cpp -w

main.o : SHT.o HT.o
	gcc -std=c++11 -c main.cpp -w
	
clean : 
	rm main.o SHT.o ../HT_lib/HT.o main

run : main
	./main
