#include <iostream>
#include <cstring>
#include <cstdlib>
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
    SHT_info info = {blockFile, attrName, attrLength, buckets, filename, sfilename, firstAllocatedBlockNum}; 
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
    int blockInserted = -1; //If insertion is successful, blockInserted will contain the block where the record was inserted
    
    HT_info HT_header_info = *HT_OpenIndex(header_info.indexFilename); //Get HT_INFO
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

    while (blockNumber != -1 && blockInserted == -1){   //While there is a block left to search and record is not inserted
        if (BF_ReadBlock(HT_header_info.fileDesc, blockNumber, &recordPtr) == -1) return -1;
        //cout << "Checking block " << blockNumber << endl;
        for(int i=1; i<=RECORDS_PER_BLOCK; i++){
            //cout << "   Checking record location " << i << endl;
            if (*static_cast<int*>(recordPtr) == 0){  //if no record is saved in this location
                memcpy(recordPtr, &record, sizeof(Record));   //save the record
                BF_WriteBlock(HT_header_info.fileDesc,blockNumber);
                //cout << "Record inserted successfully!" << endl;
                HT_CloseIndex(&HT_header_info);
                blockInserted = blockNumber;
                break;
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
    
    HT_CloseIndex(&HT_header_info);
    if (blockInserted == -1) return -1; //Insertion failed

    //Insertion successfull, insert at secondary index
    SecondaryRecord secRecord;  //Create the secondary record
    strcpy(secRecord.surname,record.surname);
    secRecord.blockId = blockInserted;

    //hashKey = ((int) secRecord.surname[8]) % header_info.numBuckets;  //Temporary, make a better hash function
    hashKey = hashFunction(secRecord.surname, header_info.numBuckets);
    //cout << hashKey << endl;
    
    ifstream SHTIndexFile(header_info.sIndexFilename);   //Open secondary index file of the hashtable and search for line number "hashKey"
    getline(SHTIndexFile, blockNumberStr);  //Skip the first line
    for (int line=0; line<=hashKey; line++){
        getline(SHTIndexFile, blockNumberStr);   //line number n contains the location of the n-th block
    }
    //cout << blockNumberStr << endl;
    blockNumber = stoi(blockNumberStr);  //block number now contains the location of the required block

    if (blockNumber == header_info.SHT_infoBlockNumber){  //if the current block is the SHT_info block, go to the next block
        blockNumber = getNextBlock(header_info.fileDesc, header_info.SHT_infoBlockNumber);
        //cout << "Skipping header block " << header_info.SHT_infoBlockNumber << endl;
    }

    void* secRecordPtr;
    while (blockNumber != -1){   //While there is a block left to search
        if (BF_ReadBlock(header_info.fileDesc, blockNumber, &secRecordPtr) == -1) return -1;
        //cout << "Checking block " << blockNumber << endl;
        for(int i=1; i<=SECONDARY_RECORDS_PER_BLOCK; i++){
            if (*static_cast<int*>(recordPtr) == 0){  //if no record is saved in this location, go sizeof(Record) bytes forward in the block file
                //cout << "   No record in record location " << i << endl;
            }
            else{  //if there is a record saved in this location, check if it is the same as the one we are inserting
                SecondaryRecord curSecRecord;
                memcpy(&curSecRecord,secRecordPtr,sizeof(SecondaryRecord)); //copy the current record from block to curRecord variable
                if (curSecRecord.surname == record.surname && curSecRecord.blockId == blockInserted) {
                    //cout << "   Record with surname " << curSecRecord.surname << " already exists in block" << blockInserted << "! Insertion skipped." << endl;
                    HT_CloseIndex(&HT_header_info);
                    return -1;
                }
                //cout << "   Record location "<< i <<" full." << endl;
            }
            recordPtr = static_cast<SecondaryRecord*>(secRecordPtr) + 1;
        }
        blockNumber = getNextBlock(header_info.fileDesc, blockNumber); //Go to the next block
        //cout << "   Next block is: " << blockNumber << endl;
    }
    //cout << "Secondary record not found! Inserting..." << endl;

    blockNumber = stoi(blockNumberStr);  //block number now contains the location of the required block
    if (blockNumber == header_info.SHT_infoBlockNumber){  //if the current block is the HT_info block, go to the next block
        void* block;
        if (BF_ReadBlock(header_info.fileDesc, header_info.SHT_infoBlockNumber, &block) == -1) return -1;
        blockNumber = getNextBlock(header_info.fileDesc, header_info.SHT_infoBlockNumber);
    }
    if (BF_ReadBlock(header_info.fileDesc, blockNumber, &secRecordPtr) == -1) return -1;

    while (blockNumber != -1){   //While there is a block left to search
        if (BF_ReadBlock(header_info.fileDesc, blockNumber, &secRecordPtr) == -1) return -1;
        //cout << "Checking block " << blockNumber << endl;
        for(int i=1; i<=SECONDARY_RECORDS_PER_BLOCK; i++){
            //cout << "   Checking secondary record location " << i << endl;
            if (*static_cast<int*>(secRecordPtr) == 0){  //if no secondary record is saved in this location
                memcpy(secRecordPtr, &secRecord, sizeof(SecondaryRecord));   //save the secondary record
                BF_WriteBlock(header_info.fileDesc,blockNumber);
                //cout << "Record inserted successfully!" << endl;
                //SHT_CloseIndex(&header_info);
                return 0;
            }
            secRecordPtr = static_cast<SecondaryRecord*>(secRecordPtr) + 1;  //go to the next record location
        }
        
        if (getNextBlock(header_info.fileDesc, blockNumber) == -1){  //if this block is the final and it is full generate a new
            //cout << "   Block full! Generating new...: " << blockNumber << endl;
            if (BF_AllocateBlock(header_info.fileDesc) == -1) return -1;
            int lastAllocatedBlock = BF_GetBlockCounter(header_info.fileDesc) - 1;
            setNextBlock(header_info.fileDesc, blockNumber, lastAllocatedBlock);
            setNextBlock(header_info.fileDesc, lastAllocatedBlock, -1);  //set the next of the new block to be NULL
        }
        //else{        //Block full, but there is another block after that
            //cout << "   Block full! Next block is: " << blockNumber << endl;
        //}
        blockNumber = getNextBlock(header_info.fileDesc, blockNumber); //Go to the next block
    }
    
    return -1;
}


int SHT_SecondaryGetAllEntries(SHT_info header_info_sht, HT_info header_info_ht, void* value){
    bool found = false;
    char* surname = (char*) value;  //search term is the surname (char*)
    int blocksExpanded = 1;
    
    //Search for the entry
    //int hashKey = ((int) surname[8]) % header_info_sht.numBuckets;  //Temporary, make a better hash function
    //cout << hashKey << endl;
    int hashKey = hashFunction(surname, header_info_sht.numBuckets);
    
    string blockNumberStr;
    ifstream SHTIndexFile(header_info_sht.sIndexFilename);   //Open index file of the hashtable and search for line number "hashKey"
    
    getline(SHTIndexFile, blockNumberStr); //skip the first line
    for (int line=0; line<=hashKey; line++){
        getline(SHTIndexFile, blockNumberStr);   //line number n contains the location of the n-th block
    }
    
    int blockNumber = stoi(blockNumberStr);  //block number now contains the location of the required block

    if (blockNumber == header_info_sht.SHT_infoBlockNumber){  //if the current block is the HT_info block, go to the next block
        blockNumber = getNextBlock(header_info_sht.fileDesc, header_info_sht.SHT_infoBlockNumber);
        //cout << "Skipping header block " << header_info_sht.SHT_infoBlockNumber << endl;
        blocksExpanded++;
    }
    

    void* secRecordPtr;
    while (blockNumber != -1){   //While there is a block left to search
        if (BF_ReadBlock(header_info_sht.fileDesc, blockNumber, &secRecordPtr) == -1) return -1;
        for(int i=1; i<=SECONDARY_RECORDS_PER_BLOCK; i++){
            SecondaryRecord curSecRecord;
            memcpy(&curSecRecord,secRecordPtr,sizeof(SecondaryRecord)); //copy the current record from block to curRecord variable
            if (strcmp(curSecRecord.surname, surname) == 0) {  //if the retrieved secondary record surname is the required, search the block it points
                //cout << "Record with surname " << surname << " found at block" << curSecRecord.blockId << endl;
                printOcurrences(header_info_sht.fileDesc, surname, curSecRecord.blockId); //SEARCH curSecRecord.blockId for every occurance of surname
                blocksExpanded++;
                found = true;
            }
            secRecordPtr = static_cast<SecondaryRecord*>(secRecordPtr) + 1;
        }
        blockNumber = getNextBlock(header_info_sht.fileDesc, blockNumber); //Go to the next block
        blocksExpanded++;
    }

    if (found) return blocksExpanded;
    else return -1;
}


int SHT_hashStatistics(char* filename){
    cout << "___________________________________________HASH STATISTICS___________________________________________" << endl;
    SHT_info secondary_info = *SHT_OpenSecondaryIndex(filename);
    int totalBlocks = 0;
    int bucketsWithOverflowBlocks = 0;
    double averageOverflowBlocks = 0;
    int minRecords = 999999;  
    int maxRecords = 0;  
    double avgRecords = 0;

    ifstream SHTIndexFile(filename);   //Open the secondary index file of the hashtable
    string blockNumberStr;
    getline(SHTIndexFile, blockNumberStr);  //Skip the first line
    while(getline(SHTIndexFile, blockNumberStr)){     //Get all lines from the index file
        int overflowBlocks = 0;
        int blockNumber = stoi(blockNumberStr);
        int firstBlock = blockNumber;

        int secRecordsInBucket = 0;
        
        if (blockNumber == secondary_info.SHT_infoBlockNumber){  //if the current block is the SHT_info block, go to the next block
            blockNumber = getNextBlock(secondary_info.fileDesc, secondary_info.SHT_infoBlockNumber);
            totalBlocks++;
        }

        while (blockNumber != -1){   //While there is a block left to search
            totalBlocks++;
            if (blockNumber != firstBlock) overflowBlocks++;  //If this block is not the first of the bucket, it is an overflow block.

            //Count the records in the current block
            void* secRecordPtr;
            BF_ReadBlock(secondary_info.fileDesc, blockNumber, &secRecordPtr);
            for(int i=1; i<=SECONDARY_RECORDS_PER_BLOCK; i++){
                SecondaryRecord curSecRecord;
                if (*static_cast<int*>(secRecordPtr) != 0){  //if a record is saved in this location
                    secRecordsInBucket++;
                }
                secRecordPtr = static_cast<SecondaryRecord*>(secRecordPtr) + 1;
            }
            blockNumber = getNextBlock(secondary_info.fileDesc, blockNumber); //Go to the next block
        }
        cout << "Overflow blocks in secondary record bucket " << firstBlock << ": " << overflowBlocks << endl;
        if (overflowBlocks>0) bucketsWithOverflowBlocks++;
        averageOverflowBlocks += static_cast<double>(overflowBlocks) / secondary_info.numBuckets;   //average overflow blocks per bucket
        if (secRecordsInBucket < minRecords) minRecords = secRecordsInBucket;
        if (secRecordsInBucket > maxRecords) maxRecords = secRecordsInBucket;
        avgRecords += static_cast<double>(secRecordsInBucket) / secondary_info.numBuckets;
    }

    cout << "Total secondary record buckets with overflow blocks: " << bucketsWithOverflowBlocks << endl;
    cout << "Average overflow blocks per secondary record bucket: " << averageOverflowBlocks << endl;
    cout << "Total secondary record blocks in file: " << totalBlocks << endl;
    cout << "Minimum secondary records per bucket: " << minRecords << endl;
    cout << "Maximum secondary records per bucket: " << maxRecords << endl;
    cout << "Average secondary records per bucket: " << avgRecords << endl;
    return 0;
}


//------------------------------------Utility Functions------------------------------------------------------//
int hashFunction(char* value, int buckets){  //source: https://stackoverflow.com/questions/7666509/hash-function-for-string
    unsigned long hash = 5381;
    int c;

    while (c = *value++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash%buckets;
}

void printOcurrences(int blockFile, char* surname, int blockId){   //Prints every ocurrence of surname in blockId (blockId is a pointer to Record block)
    void* currentBlockPtr;
    BF_ReadBlock(blockFile, blockId, &currentBlockPtr); //get a pointer to the start of the current block
    for(int i=1; i<=RECORDS_PER_BLOCK; i++){
        if (*static_cast<int*>(currentBlockPtr) != 0){ //if there is a record saved in this location, check if it is has the surname we are searching
            Record curRecord;
            memcpy(&curRecord,currentBlockPtr,sizeof(Record)); //copy the current record from block to curRecord variable
            if (strcmp(curRecord.surname, surname) == 0) {
                cout << "{" << curRecord.id << "," << curRecord.name << "," << curRecord.surname << "," << curRecord.address << "}"  << endl;
            }
        }
        currentBlockPtr = static_cast<Record*>(currentBlockPtr) + 1;  //go to the next Record
    }
}

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

