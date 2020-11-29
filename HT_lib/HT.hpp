
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
}HT_info;



int HT_CreateIndex(char* filename, char attrType, char* attrName, int attrLength, int buckets);

HT_info* HT_OpenIndex(char* filename);