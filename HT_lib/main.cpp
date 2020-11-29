#include <iostream>
#include "HT.hpp"



using namespace std;

int main(){
    HT_CreateIndex("HashTable1",'i',"Attribute",5,10);
    HT_info* HT1_info = HT_OpenIndex("HashTable1");
    
    if (HT1_info != NULL){
        cout << "HT1 fileDesc: " << HT1_info->fileDesc << endl;
        cout << "HT1 attrType: " << HT1_info->attrType << endl;
        cout << "HT1 attrName: " << HT1_info->attrName << endl;
        cout << "HT1 attrLength: " << HT1_info->attrLength << endl;
        cout << "HT1 numBuckets: " << HT1_info->numBuckets << endl;
    }
    

    return 0;
}