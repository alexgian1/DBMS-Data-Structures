#include <iostream>
#include <fstream>
#include <cstring>
#include "SHT.hpp"


using namespace std;


int main(){
    HT_CreateIndex("DB1",'i',"id",10,15);
    SHT_CreateSecondaryIndex("secondaryIndex", 's', "surname", 30, 5, "DB1");

    //Open primary and secondary index to insert records and close it
    HT_info pIndex = *HT_OpenIndex("DB1"); 
    SHT_info sIndex = *SHT_OpenSecondaryIndex("secondaryIndex");
    
    Read_From_File(sIndex,"records1K.txt");

    SHT_CloseSecondaryIndex(&sIndex);
    HT_CloseIndex(&pIndex);

    cout << "----------------------------- SEARCH TEST -----------------------------" << endl;
    
    //Primary index search
    cout << "_____Search by id_____" << endl;
    pIndex = *HT_OpenIndex("DB1");  
    int intValuesToSearch[3] = {500,150,99999};
    
    if (HT_GetAllEntries(pIndex, &intValuesToSearch[0]) != -1) cout << "Record with id " << intValuesToSearch[0] << " found!" << endl << endl;
    else cout<< "Record with id " << intValuesToSearch[0] << " not found!" << endl << endl;   
    
    if (HT_GetAllEntries(pIndex, &intValuesToSearch[1]) != -1) cout << "Record with id " << intValuesToSearch[1] << " found!" << endl << endl;
    else cout<< "Record with id " << intValuesToSearch[1] << " not found!" << endl << endl;     

    //Search non-exitent entry
    if (HT_GetAllEntries(pIndex, &intValuesToSearch[2]) != -1) cout << "Record with id " << intValuesToSearch[2] << " found!" << endl << endl;
    else cout<< "Record with id " << intValuesToSearch[2] << " not found!" << endl << endl; 
    
    
    //Secondary index search
    cout << "_____Search by surname_____" << endl;
    sIndex = *SHT_OpenSecondaryIndex("secondaryIndex");
    char* strValuesToSearch[3] = {"surname_5","surname_10","surname_X"};
    
    if (SHT_SecondaryGetAllEntries(sIndex, pIndex, strValuesToSearch[0]) != -1) cout << "Record with surname " << strValuesToSearch[0] << " found!" << endl << endl;
    else cout<< "Record with surname " << strValuesToSearch[0] << " not found!" << endl << endl;   
    
    //Search value with multiple occurences (surname_10)
    if (SHT_SecondaryGetAllEntries(sIndex, pIndex, strValuesToSearch[1]) != -1) cout << "Record with surname " << strValuesToSearch[1] << " found!" << endl << endl;
    else cout<< "Record with surname " << strValuesToSearch[1] << " not found!" << endl << endl;       

    //Search non-exitent value
    if (SHT_SecondaryGetAllEntries(sIndex, pIndex, strValuesToSearch[2]) != -1) cout << "Record with surname " << strValuesToSearch[2] << " found!" << endl << endl;
    else cout<< "Record with surname " << strValuesToSearch[2] << " not found!" << endl << endl;    

    HT_CloseIndex(&pIndex);
    SHT_CloseSecondaryIndex(&sIndex);

    //-------------------------------------------------------------------------------------------------------------------------------//

    
    
    return 0;
}

