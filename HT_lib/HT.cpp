#include <iostream>
#include <cstring>
#include <fstream>
#include "HT.hpp"
extern "C"{  //link with C library
    #include "../BF_lib/BF.h"  
}

using namespace std;

int HT_CreateIndex(string filename, char attrType, string attrName, int attrLength, int buckets){
    BF_Init();
    BF_CreateFile("Blocks");
    int blockFile = BF_OpenFile("Blocks");

    HT_info info = {blockFile, attrType, attrName, attrLength, buckets};  //add HT_info at the first block
    void* HTinfoBlock;
    BF_AllocateBlock(blockFile);  //allocate a block for HT_INFO
    BF_ReadBlock(blockFile,0,&HTinfoBlock);  //store HT_info block location to HTinfoBlock pointer
    memcpy(HTinfoBlock, &info, sizeof(HT_info));  //copy HT_info contents to HTinfoBlock
    BF_WriteBlock(blockFile,0);
    


    
    return 0;
}