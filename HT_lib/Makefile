main : main.o HT.o
	gcc -o main HT.o main.o ../BF_lib/BF_64.a -no-pie -lstdc++


main.o : HT.cpp HT.hpp
	gcc -c main.cpp -lstdc++

HT.o : HT.cpp HT.hpp ../BF_lib/BF_64.a
	gcc -c HT.cpp -lstdc++

clean : 
	rm main.o HT.o 

run : main
	./main
