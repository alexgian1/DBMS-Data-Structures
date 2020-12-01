#include <iostream>
#include <cstring>
#include <fstream>
#include "HT.hpp"
extern "C"{  //link with C library
    #include "../BF_lib/BF.h"  
}

#define RECORDS_PER_BLOCK 5

using namespace std;
//------------------------------------Utility Functions------------------------------------------------------//
int hashFunction(int id, int buckets){
    return id%buckets;
}

int getNextBlock(int blockFile, int currentBlock){
    //Returns the index of the next block. If there is no block after the current, returns -1.
    int nextBlockIndex;
    void* currentBlockPtr;
    
    BF_ReadBlock(blockFile,currentBlock, &currentBlockPtr); //get a pointer to the start of the current block
    void* lastBlockBytes = static_cast<char*>(currentBlockPtr) + 512 - sizeof(int);  //point to the last 8 bytes (size of int) of the block
    if (lastBlockBytes == NULL) return -1;
    cout << "Searching for block at: " << lastBlockBytes << endl;
    memcpy(&nextBlockIndex, lastBlockBytes, sizeof(int));  //copy the index of the next block to the 
    cout << "reached" << endl;
    return nextBlockIndex;
}

void setNextBlock(int blockFile, int currentBlock, int nextBlock){
    //Sets the index of the next block.
    void* currentBlockPtr;
    
    BF_ReadBlock(blockFile,currentBlock, &currentBlockPtr); //get a pointer to the start of the current block
    void* lastBlockBytes = static_cast<char*>(currentBlockPtr) + 512 - sizeof(int);  //point to the last 8 bytes (size of int) of the block
    memcpy(lastBlockBytes, &nextBlock, sizeof(int));  //copy the index of the next block to the end of the block
    cout << "reached" << endl;
}

//-----------------------------------------------------------------------------------------------------------//

int HT_CreateIndex(char* filename, char attrType, char* attrName, int attrLength, int buckets){
    //Create a hashtable index:
    //The n-th line of the file is the location of the n-th bucket of the hash table
    string HTIndexFilename = "index";
    //HTIndexFilename.append("Index");   // Add "Index" at the end of filename
    ofstream HTIndex(HTIndexFilename);

    //Create the blockfile and allocate blocks
    BF_Init();
    if (BF_CreateFile(filename) < 0) return -1;

    int blockFile;
    if ( (blockFile = BF_OpenFile(filename)) < 0 ) return -1;

    for (int i=1; i<=buckets; i++){
        if (BF_AllocateBlock(blockFile) < 0) return -1;  //Allocate 1 empty block for every bucket
        HTIndex << BF_GetBlockCounter(blockFile)-1 << endl; //Add the index of the last allocated block to HTIndex file
        setNextBlock(blockFile, BF_GetBlockCounter(blockFile)-1, -1);  //Set the next block to be -1

    }

    //add HT_info at the first block
    //-------PROBLEM: HTIndexFilename NOT SAVED if a variable is passed instead of "index"
    
    HT_info info = {blockFile, attrType, attrName, attrLength, buckets, "index"}; 
    void* HTinfoBlock;
    if (BF_ReadBlock(blockFile,0,&HTinfoBlock) < 0) return -1;  //store HT_info block location to HTinfoBlock pointer
    memcpy(HTinfoBlock, &info, sizeof(HT_info));  //copy HT_info contents to HTinfoBlock

    if (BF_AllocateBlock(blockFile) < 0) return -1;   //Allocate a new block after HT_info block
    int lastAllocatedBlock = BF_GetBlockCounter(blockFile)-1;   //The index of the new block
    
    setNextBlock(blockFile,0,lastAllocatedBlock);  //set the next block to be the last allocated block
    /*
    cout << "First block is: " << HTinfoBlock << endl;
    void* lastBlockBytes = static_cast<char*>(HTinfoBlock) + 512 -sizeof(int);  //point to the last 8 bytes (size of int) of the block
    cout << "Next block saved at: " << lastBlockBytes << endl;
    memcpy(lastBlockBytes, &lastAllocatedBlock, sizeof(int));  //copy the index of the allocated block to the last 8 bytes of HT_info block
    */

    if (BF_WriteBlock(blockFile,0) < 0) return -1;

    HTIndex.close();
    
    return 0;   //No error => return 0
}


HT_info* HT_OpenIndex(char* filename){
    int blockFile;
    if ( (blockFile = BF_OpenFile(filename)) < 0) return NULL;  //open the file containing the blocks
    void* HTinfoBlock;
    if (BF_ReadBlock(blockFile,0,&HTinfoBlock) < 0) return NULL;  //store HT_info block location to HTinfoBlock pointer (HT_info is block 0 of the file)
    
    HT_info* infoPtr = (HT_info*) malloc(sizeof(HT_info)); //allocate an HT_info struct and assign a pointer to it
    if (infoPtr == NULL) return NULL;  //If malloc could not allocate space for infoPtr return error
    memcpy(infoPtr,HTinfoBlock,sizeof(HT_info));  //store HT_info block contents to the allocated struct which infoPtr points

    return infoPtr;  //return the location of the retrieved info
    //TODO: Check if the file provided IS a hash table file
}


int HT_CloseIndex(HT_info* header_info){
    if (BF_CloseFile(header_info->fileDesc) < 0) return -1;
    free(header_info);
    return 0;
}


int HT_InsertEntry(HT_info header_info, Record record){
    //Search for the entry 
    bool found = 0;
    int hashKey = hashFunction(record.id, header_info.numBuckets);
    string blockNumberStr;
    ifstream HTIndexFile(header_info.indexFilename);   //Open index file of the hashtable and search for line number "hashKey"
    for (int line=0; line<=hashKey; line++){
        getline(HTIndexFile, blockNumberStr);   //line number n contains the location of the n-th block
        cout << "HT_Index block: " <<blockNumberStr << endl;
    }
    
    int blockNumber = stoi(blockNumberStr);  //block number now contains the location of the required block

    if (blockNumber == 0){  //if the current block is the HT_info block, go to the next block
        void* block;
        BF_ReadBlock(header_info.fileDesc, 0, &block);
        cout << "First block is: " << block << endl;
        blockNumber = getNextBlock(header_info.fileDesc, 0);
    }
    

    void* recordPtr;
    BF_ReadBlock(header_info.fileDesc, blockNumber, &recordPtr);
    while (blockNumber != -1){   //While there is a block left to search
        for(int i=1; i<=RECORDS_PER_BLOCK; i++){
            if (recordPtr == NULL){  //if no record is saved in this location, go sizeof(Record) bytes forward in the block file
                recordPtr = static_cast<Record*>(recordPtr) + 1;
            }
            else{  //if there is a record saved in this location, check if it is the same as the one we are inserting
                Record curRecord;
                memcpy(&curRecord,recordPtr,sizeof(Record)); //copy the current record from block to curRecord variable
                if (curRecord.id == record.id) found = 1;  //if the record in the block has the same id to the one we are inserting: found=true
            }
        }
        blockNumber = getNextBlock(header_info.fileDesc, blockNumber); //Go to the next block
    }

    if (found) cout << "Record found!" << endl;
    else cout << "Record not found!" << endl;
    

    
    return 0;
}