#include <string>
#include "../HT_lib/HT.hpp"

typedef struct{
    int fileDesc;
    char* attrName;
    int attrLength;
    long int numBuckets;
    char* indexFilename;
    int SHT_infoBlockNumber;
}SHT_info;

typedef struct{
    char surname[25];
    int blockId;
}SecondaryRecord;


int SHT_CreateSecondaryIndex(char* sfilename, char attrType, char* attrName, int attrLength, int buckets, char* filename);

SHT_info* SHT_OpenSecondaryIndex(char* sfilename);

int SHT_CloseSecondaryIndex(SHT_info* header_info);

int SHT_SecondaryInsertEntry(SHT_info header_info, SecondaryRecord record);

/*
int SHT_GetAllEntries(SHT_info header_info, void* value);

int hashStatistics(char* filename);

*/
//----------------Utility Functions----------------//

//int hashFunction(int id, int buckets);
//int getNextBlock(int blockFile, int currentBlock);
//void setNextBlock(int blockFile, int currentBlock, int nextBlock);
void Read_From_File(SHT_info header_info, std::string recordsFile);  //Used in main()