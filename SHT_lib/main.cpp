#include <iostream>
#include <fstream>
#include <cstring>
#include "SHT.hpp"
#include "../HT_lib/HT.hpp"

using namespace std;


int main(){
    HT_CreateIndex("DB1",'i',"id",10,15);
    SHT_CreateIndex("secondaryIndex", 's', "surname", 30, 5, "DB1");
    
    HT_info pIndexPtr = *HT_OpenIndex("DB1");
    SHT_info sIndexPtr = *SHT_OpenIndex("secondaryIndex");
    cout << "_______PRIMARY INDEX_______" << endl;
    cout << "fileDesc = " << pIndexPtr.fileDesc << endl;
    cout << "attrName = " << pIndexPtr.attrName << endl;
    cout << "attrLength = " << pIndexPtr.attrLength << endl;
    cout << "numBuckets = " << pIndexPtr.numBuckets << endl;
    cout << "indexFilename = " << pIndexPtr.indexFilename << endl;

    cout << "_______SECONDARY INDEX_______" << endl;
    cout << "fileDesc = " << sIndexPtr.fileDesc << endl;
    cout << "attrName = " << sIndexPtr.attrName << endl;
    cout << "attrLength = " << sIndexPtr.attrLength << endl;
    cout << "numBuckets = " << sIndexPtr.numBuckets << endl;
    cout << "indexFilename = " << sIndexPtr.indexFilename << endl;

    SHT_CloseIndex(&sIndexPtr);
    return 0;
}

