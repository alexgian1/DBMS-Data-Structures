#include <iostream>
#include <cstring>
#include <fstream>
#include "SHT.hpp"

extern "C"{  //link with C library
    #include "../BF_lib/BF.h"  
}


#define RECORDS_PER_BLOCK 5
#define SECONDARY_RECORDS_PER_BLOCK 14

using namespace std;

//-----------------------------------------------------------------------------------------------------------//

int SHT_CreateSecondaryIndex(char* sfilename, char attrType, char* attrName, int attrLength, int buckets, char* filename){
    //Create the secondary index file:
    //each line contains the block number where the key-pointer are saved
    
    ofstream SHTIndex(sfilename);
    SHTIndex << filename << endl;

    int blockFile;
    if ( (blockFile = BF_OpenFile(filename)) < 0 ) return -1;
    for (int i=1; i<=buckets; i++){
        if (BF_AllocateBlock(blockFile) < 0) return -1;  //Allocate 1 empty block for every bucket
        SHTIndex << BF_GetBlockCounter(blockFile)-1 << endl; //Add the index of the last allocated block to SHTIndex file
        //cout << "Allocated block " << BF_GetBlockCounter(blockFile)-1 << endl;
        setNextBlock(blockFile, BF_GetBlockCounter(blockFile)-1, -1);  //Set the next block to be -1
    }
    
    int firstAllocatedBlockNum = BF_GetBlockCounter(blockFile)-buckets; //the first block allocated for the first bucket
    //add HT_info at the first block    
    SHT_info info = {blockFile, attrName, attrLength, buckets, filename, firstAllocatedBlockNum}; 
    void* SHTinfoBlock;
    //cout << "SHT_info saved at block " << BF_GetBlockCounter(blockFile)-buckets << endl;
    if (BF_ReadBlock(blockFile,firstAllocatedBlockNum,&SHTinfoBlock) < 0) return -1;  //store SHT_info block location to HTinfoBlock pointer
    memcpy(SHTinfoBlock, &info, sizeof(SHT_info));  //copy SHT_info contents to SHTinfoBlock
    
    if (BF_AllocateBlock(blockFile) < 0) return -1;   //Allocate a new block after HT_info block
    int lastAllocatedBlock = BF_GetBlockCounter(blockFile)-1;   //The index of the new block
    setNextBlock(blockFile,lastAllocatedBlock,-1); //set the next block of the last allocated block to be NULL
    
    setNextBlock(blockFile,firstAllocatedBlockNum,lastAllocatedBlock);  //set the next block of HT_INFO block to be the last allocated block

    if (BF_WriteBlock(blockFile,0) < 0) return -1;
   
    SHTIndex.close();
    
    return 0;   //No error => return 0
}


SHT_info* SHT_OpenSecondaryIndex(char* sfilename){
    //Get the block file name
    ifstream SHTIndex(sfilename);    
    string filenameStr;
    getline(SHTIndex, filenameStr);  
    char* filename = strdup(filenameStr.c_str());

    //Get the first SHT block (contains SHT_info)
    int infoBlockNum;
    SHTIndex >> infoBlockNum;   //the first block is at the 2nd line of the file
    //cout << "SHT info block num: " << infoBlockNum << endl;

    //Open the block file and retrieve SHT_info
    int blockFile;
    if ( (blockFile = BF_OpenFile(filename)) < 0) return NULL;  //open the file containing the blocks
    void* SHTinfoBlock;
    if (BF_ReadBlock(blockFile,infoBlockNum,&SHTinfoBlock) < 0) return NULL;  //store HT_info block location to HTinfoBlock pointer (HT_info is block 0 of the file)
    
    SHT_info* infoPtr = new(SHT_info); //allocate an HT_info struct and assign a pointer to it
    if (infoPtr == NULL) return NULL;  //If new could not allocate space for infoPtr return error
    memcpy(infoPtr,SHTinfoBlock,sizeof(SHT_info));  //store HT_info block contents to the allocated struct which infoPtr points

    return infoPtr;  //return the location of the retrieved info
}


int SHT_CloseSecondaryIndex(SHT_info* header_info){
    if (BF_CloseFile(header_info->fileDesc) < 0) return -1;
    //free(header_info);
    return 0;
}


int SHT_SecondaryInsertEntry(SHT_info header_info, Record record){
    //cout << "Number of blocks: " << BF_GetBlockCounter(header_info.fileDesc) << endl;
    
    HT_info HT_header_info = *HT_OpenIndex(header_info.indexFilename); //Get HT_INFO
    //cout << "reached" << endl;
    //Search for the entry in primary index
    int hashKey = hashFunction(record.id, HT_header_info.numBuckets);
    string blockNumberStr;
    ifstream HTIndexFile(HT_header_info.indexFilename);   //Open index file of the hashtable and search for line number "hashKey"
    for (int line=0; line<=hashKey; line++){
        getline(HTIndexFile, blockNumberStr);   //line number n contains the location of the n-th block
    }
    
    int blockNumber = stoi(blockNumberStr);  //block number now contains the location of the required block
    
    if (blockNumber == 0){  //if the current block is the HT_info block, go to the next block
        blockNumber = getNextBlock(HT_header_info.fileDesc, 0);
        //cout << "Skipping header block 0" << endl;
    }
    

    void* recordPtr;
    while (blockNumber != -1){   //While there is a block left to search
        if (BF_ReadBlock(HT_header_info.fileDesc, blockNumber, &recordPtr) == -1) return -1;
        //cout << "Checking block " << blockNumber << endl;
        for(int i=1; i<=RECORDS_PER_BLOCK; i++){
            if (*static_cast<int*>(recordPtr) == 0){  //if no record is saved in this location, go sizeof(Record) bytes forward in the block file
                //cout << "   No record in record location " << i << endl;
            }
            else{  //if there is a record saved in this location, check if it is the same as the one we are inserting
                Record curRecord;
                memcpy(&curRecord,recordPtr,sizeof(Record)); //copy the current record from block to curRecord variable
                if (curRecord.id == record.id) {
                    //cout << "   Record with id " << curRecord.id << " already exists! Insertion failed." << endl;
                    HT_CloseIndex(&HT_header_info);
                    return -1;
                }
                //cout << "   Record location "<< i <<" full." << endl;
            }
            recordPtr = static_cast<Record*>(recordPtr) + 1;
        }
        blockNumber = getNextBlock(HT_header_info.fileDesc, blockNumber); //Go to the next block
        //cout << "   Next block is: " << blockNumber << endl;
    }
    //cout << "Record not found! Inserting..." << endl;

    //Insert the entry in primary index
    //Find the 1st empty record location

    blockNumber = stoi(blockNumberStr);  //block number now contains the location of the required block
    if (blockNumber == 0){  //if the current block is the HT_info block, go to the next block
        void* block;
        if (BF_ReadBlock(HT_header_info.fileDesc, 0, &block) == -1) return -1;
        blockNumber = getNextBlock(HT_header_info.fileDesc, 0);
    }
    if (BF_ReadBlock(HT_header_info.fileDesc, blockNumber, &recordPtr) == -1) return -1;

    while (blockNumber != -1){   //While there is a block left to search
        if (BF_ReadBlock(HT_header_info.fileDesc, blockNumber, &recordPtr) == -1) return -1;
        //cout << "Checking block " << blockNumber << endl;
        for(int i=1; i<=RECORDS_PER_BLOCK; i++){
            //cout << "   Checking record location " << i << endl;
            if (*static_cast<int*>(recordPtr) == 0){  //if no record is saved in this location
                memcpy(recordPtr, &record, sizeof(Record));   //save the record
                BF_WriteBlock(HT_header_info.fileDesc,blockNumber);
                //cout << "Record inserted successfully!" << endl;
                HT_CloseIndex(&HT_header_info);
                return 0;
            }
            recordPtr = static_cast<Record*>(recordPtr) + 1;  //go to the next record location
        }
        
        if (getNextBlock(HT_header_info.fileDesc, blockNumber) == -1){  //if this block is the final and it is full generate a new
            //cout << "   Block full! Generating new...: " << blockNumber << endl;
            if (BF_AllocateBlock(HT_header_info.fileDesc) == -1) return -1;
            int lastAllocatedBlock = BF_GetBlockCounter(HT_header_info.fileDesc) - 1;
            setNextBlock(HT_header_info.fileDesc, blockNumber, lastAllocatedBlock);
            setNextBlock(HT_header_info.fileDesc, lastAllocatedBlock, -1);  //set the next of the new block to be NULL
        }
        //else{        //Block full, but there is another block after that
            //cout << "   Block full! Next block is: " << blockNumber << endl;
        //}
        blockNumber = getNextBlock(HT_header_info.fileDesc, blockNumber); //Go to the next block
    }
    //TODO : Add surname with block pointer at secondary index 

    HT_CloseIndex(&HT_header_info);
    return -1;
}



/*
int HT_GetAllEntries(HT_info header_info, void* value){
    int id = *(static_cast<int*>(value));  //primary key is integer
    int blocksExpanded = 1;
    
    //Search for the entry 
    int hashKey = hashFunction(id, header_info.numBuckets);
    string blockNumberStr;
    ifstream HTIndexFile(header_info.indexFilename);   //Open index file of the hashtable and search for line number "hashKey"
    for (int line=0; line<=hashKey; line++){
        getline(HTIndexFile, blockNumberStr);   //line number n contains the location of the n-th block
    }
    
    int blockNumber = stoi(blockNumberStr);  //block number now contains the location of the required block

    if (blockNumber == 0){  //if the current block is the HT_info block, go to the next block
        blockNumber = getNextBlock(header_info.fileDesc, 0);
        //cout << "Skipping header block 0" << endl;
        blocksExpanded++;
    }
    

    void* recordPtr;
    while (blockNumber != -1){   //While there is a block left to search
        if (BF_ReadBlock(header_info.fileDesc, blockNumber, &recordPtr) == -1) return -1;
        for(int i=1; i<=RECORDS_PER_BLOCK; i++){
            Record curRecord;
            memcpy(&curRecord,recordPtr,sizeof(Record)); //copy the current record from block to curRecord variable
            if (curRecord.id == id) {  //if the retrieved record is the required, search is completed
                //cout << "Record with id " << id << " found!" << endl;
                //cout << "{" << curRecord.id << "," << curRecord.name << "," << curRecord.surname << "," << curRecord.address << "}"  << endl;
                //cout << "Expanded " << blocksExpanded << " blocks." << endl;
                return blocksExpanded;
            }
            recordPtr = static_cast<Record*>(recordPtr) + 1;
        }
        blockNumber = getNextBlock(header_info.fileDesc, blockNumber); //Go to the next block
        blocksExpanded++;
    }
    //cout << "Record with id " << id << " not found!" << endl;
    return -1;
}

*/
/*
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
    memcpy(&nextBlockIndex, lastBlockBytes, sizeof(int));  //copy the index of the next block to the
    return nextBlockIndex;
}

void setNextBlock(int blockFile, int currentBlock, int nextBlock){
    //Sets the index of the next block.
    void* currentBlockPtr;
    BF_ReadBlock(blockFile,currentBlock, &currentBlockPtr); //get a pointer to the start of the current block
    void* lastBlockBytes = static_cast<char*>(currentBlockPtr) + 512 - sizeof(int);  //point to the last 8 bytes (size of int) of the block
    memcpy(lastBlockBytes, &nextBlock, sizeof(int));  //copy the index of the next block to the end of the block
    BF_WriteBlock(blockFile,currentBlock);
}
*/
void Read_From_File(SHT_info header_info, string recordsFile){

	Record rec;
	ifstream myReadFile;
    string recordsFolder = "../record_examples/";
    myReadFile.open(recordsFolder + recordsFile);
 	string output;
 	if (myReadFile.is_open()) {

 		while (!myReadFile.eof()) {
    		myReadFile >> output;

    		std::size_t pos = output.find(",");
    		std::string id = output.substr (1,pos);

    		std::string remaining = output.substr (pos+2);
    		std::size_t pos2 = remaining.find(",");
    		pos2--;
    		std::string name = output.substr (pos+2 ,pos2);


    		remaining = remaining.substr (pos2+3);
    		std::size_t pos3 = remaining.find(",");
    		pos3--;
    		std::string surname = remaining.substr (0 ,pos3);


    		remaining = remaining.substr (pos3+3);
    		std::size_t pos4 = remaining.find("}");
    		pos3--;
    		std::string address = remaining.substr (0 ,pos4-1);

    		int int_id= stoi(id);

    		rec.id=int_id;
    		strcpy(rec.name,name.c_str());
    		strcpy(rec.surname,surname.c_str());
    		strcpy(rec.address,address.c_str());
        
    		if (SHT_SecondaryInsertEntry(header_info,rec) != -1) cout << "Record with id " << rec.id << " inserted successfully!" << endl;
 		}
	}
	myReadFile.close();
}

