#include <iostream>
#include <string>
#include <fstream>
#include "HT.hpp"
extern "C"{  //link with C library
    #include "../BF_lib/BF.h"  
}


using namespace std;

int HT_CreateIndex(string filename, char attrType, string attrName, int attrLength, int buckets){
    ofstream HT_File(filename);
    HT_File.close();

    //Test
    BF_CreateFile("TEST");
    
    return 0;
}