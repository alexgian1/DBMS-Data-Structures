#include <iostream>
#include <fstream>
#include <cstring>
#include "HT.hpp"

using namespace std;


int main(){

    if (HT_CreateIndex("HashTable1",'i',"Attribute",5,300) == 0){
        cout << "File created sucessfully!" << endl;
    }

    HT_info* HT1_info = HT_OpenIndex("HashTable1");
    
    if (HT1_info != NULL){
        cout << "HT1 fileDesc: " << HT1_info->fileDesc << endl;
        cout << "HT1 attrType: " << HT1_info->attrType << endl;
        cout << "HT1 attrName: " << HT1_info->attrName << endl;
        cout << "HT1 attrLength: " << HT1_info->attrLength << endl;
        cout << "HT1 numBuckets: " << HT1_info->numBuckets << endl;
        cout << "HT1 index: " << HT1_info->indexFilename << endl;
    }

    Read_From_File(*HT1_info,"records5K.txt");

    cout << "----------------------------- DELETE TEST -----------------------------" << endl;

    int valuesToDelete[4] = {150,12,2400,54445};
    HT_DeleteEntry(*HT1_info, &valuesToDelete[0]);
    HT_DeleteEntry(*HT1_info, &valuesToDelete[1]);
    HT_DeleteEntry(*HT1_info, &valuesToDelete[2]);
    HT_DeleteEntry(*HT1_info, &valuesToDelete[2]);  //Delete already delete entry
    HT_DeleteEntry(*HT1_info, &valuesToDelete[3]);  //Delete non-existent entry

    cout << "----------------------------- SEARCH TEST -----------------------------" << endl;

    int valuesToSearch[3] = {500,150,99999};
    HT_GetAllEntries(*HT1_info, &valuesToSearch[0]);
    HT_GetAllEntries(*HT1_info, &valuesToSearch[1]);  //Search deleted entry
    HT_GetAllEntries(*HT1_info, &valuesToSearch[2]);  //Search non-exitent entry

    hashStatistics("HashTable1");

    HT_CloseIndex(HT1_info);

    return 0;
}

