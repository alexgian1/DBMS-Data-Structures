main :  HP.o main.o
	gcc -o main HP.o main.o ../BF_lib/BF_64.a -no-pie -lstdc++

main.o : HP.cpp HP_lib.hpp 
	gcc -std=c++11 -c main.cpp -w

HP.o : HP.cpp HP_lib.hpp
	gcc -std=c++11 -c HP.cpp 

run: main
	./main

clean : 
	rm main.o HP.o main 
