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
    std::string attrName;
    int attrLength;
    long int numBuckets;
}HT_info;

int HT_CreateIndex(std::string filename, char attrType, std::string attrName, int attrLength, int buckets);