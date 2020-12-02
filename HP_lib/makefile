main :  HP.o
	gcc -o HP HP.o ../BF_lib/BF_64.a -no-pie -lstdc++


HP.o : HP.cpp HP_lib.hpp
	gcc -c HP.cpp -lstdc++



clean : 
	rm  HP.o 
