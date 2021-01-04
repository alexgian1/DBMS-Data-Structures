#include <string>

typedef struct{
    int fileDesc;
    char* attrName;
    int attrLength;
    long int numBuckets;
    char* indexFilename;
}SHT_info;


int SHT_CreateIndex(char* sfilename, char attrType, char* attrName, int attrLength, int buckets, char* filename);

SHT_info* SHT_OpenIndex(char* sfilename);

int SHT_CloseIndex(SHT_info* header_info);
/*
int HT_InsertEntry(HT_info header_info, Record record);

int HT_DeleteEntry(HT_info header_info, void* value);

int HT_GetAllEntries(HT_info header_info, void* value);

int hashStatistics(char* filename);

*/
//----------------Utility Functions----------------//

//int hashFunction(int id, int buckets);
//int getNextBlock(int blockFile, int currentBlock);
//void setNextBlock(int blockFile, int currentBlock, int nextBlock);
//void Read_From_File(HT_info header_info, std::string recordsFile);  //Used in main()