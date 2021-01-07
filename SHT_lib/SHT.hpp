#include <string>
#include "../HT_lib/HT.hpp"

typedef struct{
    int fileDesc;
    char* attrName;
    int attrLength;
    long int numBuckets;
    char* indexFilename;
    char* sIndexFilename;
    int SHT_infoBlockNumber;
}SHT_info;

typedef struct{
    char surname[25];
    int blockId;
}SecondaryRecord;


int SHT_CreateSecondaryIndex(char* sfilename, char attrType, char* attrName, int attrLength, int buckets, char* filename);

SHT_info* SHT_OpenSecondaryIndex(char* sfilename);

int SHT_CloseSecondaryIndex(SHT_info* header_info);

int SHT_SecondaryInsertEntry(SHT_info header_info, Record record);

int SHT_SecondaryGetAllEntries(SHT_info header_info_sht, HT_info header_info_ht, void* value);

/*
int hashStatistics(char* filename);

*/
//----------------Utility Functions----------------//

int hashFunction(char* value, int buckets);

void printOcurrences(int blockFile, char* surname, int blockId);

void Read_From_File(SHT_info header_info, std::string recordsFile);  //Used in main()