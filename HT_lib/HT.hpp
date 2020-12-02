#include <string>

typedef struct{
    int id;
    char name[15];
    char surname[25];
    char address[50];
}Record;

typedef struct{
    int fileDesc;
    char attrType;
    char* attrName;
    int attrLength;
    long int numBuckets;
    char* indexFilename;
}HT_info;


int HT_CreateIndex(char* filename, char attrType, char* attrName, int attrLength, int buckets);

HT_info* HT_OpenIndex(char* filename);

int HT_CloseIndex(HT_info* header_info);

int HT_InsertEntry(HT_info header_info, Record record);

int HT_DeleteEntry(HT_info header_info, void* value);

int HT_GetAllEntries(HT_info header_info, void* value);

