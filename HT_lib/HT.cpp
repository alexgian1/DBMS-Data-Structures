#include <iostream>
#include <cstring>
#include "HT.hpp"
extern "C"{  //link with C library
    #include "../BF_lib/BF.h"  
}

using namespace std;

int HT_CreateIndex(char* filename, char attrType, char* attrName, int attrLength, int buckets){
    BF_Init();
    if (BF_CreateFile(filename) != 0) return -1;

    int blockFile;
    if ( (blockFile = BF_OpenFile(filename)) != 0 ) return -1;

    HT_info info = {blockFile, attrType, attrName, attrLength, buckets};  //add HT_info at the first block
    void* HTinfoBlock;
    if (BF_AllocateBlock(blockFile) < 0) return -1;  //allocate a block for HT_INFO
    if (BF_ReadBlock(blockFile,0,&HTinfoBlock) < 0) return -1;  //store HT_info block location to HTinfoBlock pointer
    memcpy(HTinfoBlock, &info, sizeof(HT_info));  //copy HT_info contents to HTinfoBlock
    if (BF_WriteBlock(blockFile,0) < 0) return -1;
    
    for (int i=1; i<=buckets; i++){
        if (BF_AllocateBlock(blockFile) < 0) return -1;  //Allocate 1 empty block for every bucket
    }
    return 0;   //No error => return 0
}

HT_info* HT_OpenIndex(char* filename){
    int blockFile;
    if ( (blockFile = BF_OpenFile(filename)) < 0) return NULL;  //open the file containing the blocks
    void* HTinfoBlock;
    if (BF_ReadBlock(blockFile,0,&HTinfoBlock) < 0) return NULL;  //store HT_info block location to HTinfoBlock pointer (HT_info is block 0 of the file)
    
    HT_info* infoPtr = (HT_info*) malloc(sizeof(HT_info)); //allocate an HT_info struct and assign a pointer to it
    memcpy(infoPtr,HTinfoBlock,sizeof(HT_info));  //store HT_info block contents to the allocated struct which infoPtr points;

    return infoPtr;  //return the location of the retrieved info
    //TODO: Check if the file provided IS a hash table file
}