#include <iostream>
#include <fstream>
#include "HT.hpp"

using namespace std;



int main(){

    if (HT_CreateIndex("HashTable1",'i',"Attribute",5,10) == 0){
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
    
    /*
    if (HT_CloseIndex(HT1_info) == 0){
        cout << "File closed sucessfully!" << endl;
    }*/
    
    /*
    
    Record record1 = {150,"Name1","Surname1","Address1"};
    Record record2 = {140,"Name2","Surname2","Address2"};
    Record record3 = {130,"Name3","Surname3","Address3"};
    Record record4 = {120,"Name3","Surname3","Address3"};
    Record record5 = {110,"Name3","Surname3","Address3"};
    Record record6 = {100,"Name3","Surname3","Address3"};

    HT_InsertEntry(*HT1_info, record1);
    cout << "---------------------------------"<< endl;
    HT_InsertEntry(*HT1_info, record2);
    cout << "---------------------------------"<< endl;
    HT_InsertEntry(*HT1_info, record3);
    cout << "---------------------------------"<< endl;
    HT_InsertEntry(*HT1_info, record4);
    cout << "---------------------------------"<< endl;
    HT_InsertEntry(*HT1_info, record5);
    cout << "---------------------------------"<< endl;
    HT_InsertEntry(*HT1_info, record6);
    cout << "---------------------------------"<< endl;
    cout << "---------------------------------"<< endl;
    HT_InsertEntry(*HT1_info, record1);
    cout << "---------------------------------"<< endl;
    HT_InsertEntry(*HT1_info, record2);
    cout << "---------------------------------"<< endl;
    HT_InsertEntry(*HT1_info, record3);
    
    */

   
   for (int i=1; i<=160; i+=1){
        cout << "------------------------------------" << i <<"-----------------------------------------------------" << endl;
       Record recordi = {i,"Name1","Surname1","Address1"};
       HT_InsertEntry(*HT1_info, recordi);
      
   }

   for (int i=171; i<=400; i+=10){
        cout << "------------------------------------" << i <<"-----------------------------------------------------" << endl;
       Record recordi = {i,"Name1","Surname1","Address1"};
       HT_InsertEntry(*HT1_info, recordi);
   }

    Record record145 = {145,"Name1","Surname1","Address1"};
    int value = 145;
    HT_InsertEntry(*HT1_info, record145);
    HT_GetAllEntries(*HT1_info, &value);
    HT_DeleteEntry(*HT1_info, &value);
    HT_GetAllEntries(*HT1_info, &value);
    HT_InsertEntry(*HT1_info, record145);
   
    hashStatistics("HashTable1");    


    return 0;
}