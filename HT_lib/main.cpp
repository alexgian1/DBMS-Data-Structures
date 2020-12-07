#include <iostream>
#include <fstream>
#include <cstring>
#include "HT.hpp"

using namespace std;


int main(){

    if (HT_CreateIndex("HashTable1",'i',"Attribute",5,100) == 0){
        cout << "File created sucessfully!" << endl;
    }

    HT_info* HT1_info = HT_OpenIndex("HashTable1");

    Read_From_File(*HT1_info,"records5K.txt");


    cout << "----------------------------- DELETE TEST -----------------------------" << endl;

    int valuesToDelete[4] = {150,12,2400,54445};

    if (HT_DeleteEntry(*HT1_info, &valuesToDelete[0]) != -1) cout << "Record with id " << valuesToDelete[0] << " deleted successfully!" << endl;
    else cout<< "Error deleting record with id " << valuesToDelete[0] << endl;

    if (HT_DeleteEntry(*HT1_info, &valuesToDelete[1]) != -1) cout << "Record with id " << valuesToDelete[1] << " deleted successfully!" << endl;
    else cout<< "Error deleting record with id " << valuesToDelete[1] << endl;

    if (HT_DeleteEntry(*HT1_info, &valuesToDelete[2]) != -1) cout << "Record with id " << valuesToDelete[2] << " deleted successfully!" << endl;
    else cout<< "Error deleting record with id " << valuesToDelete[2] << endl;

    //Delete already deleted entry
    if (HT_DeleteEntry(*HT1_info, &valuesToDelete[2]) != -1) cout << "Record with id " << valuesToDelete[2] << " deleted successfully!" << endl;
    else cout<< "Error deleting record with id " << valuesToDelete[2] << endl; 
    
    //Delete non-existent entry
    if (HT_DeleteEntry(*HT1_info, &valuesToDelete[3]) != -1) cout << "Record with id " << valuesToDelete[3] << " deleted successfully!" << endl;
    else cout<< "Error deleting record with id " << valuesToDelete[3] << endl;   


    cout << "----------------------------- SEARCH TEST -----------------------------" << endl;

    int valuesToSearch[3] = {500,150,99999};
    
    if (HT_GetAllEntries(*HT1_info, &valuesToSearch[0]) != -1) cout << "Record with id " << valuesToSearch[0] << " found!" << endl;
    else cout<< "Record with id " << valuesToSearch[0] << " not found!" << endl;   
    
    //Search deleted entry
    if (HT_GetAllEntries(*HT1_info, &valuesToSearch[1]) != -1) cout << "Record with id " << valuesToSearch[1] << " found!" << endl;
    else cout<< "Record with id " << valuesToSearch[1] << " not found!" << endl;     

    //Search non-exitent entry
    if (HT_GetAllEntries(*HT1_info, &valuesToSearch[2]) != -1) cout << "Record with id " << valuesToSearch[2] << " found!" << endl;
    else cout<< "Record with id " << valuesToSearch[2] << " not found!" << endl;    

    //-------------------------------------------------------------------------------------------------------------------------------//

    hashStatistics("HashTable1");

    HT_CloseIndex(HT1_info);

    return 0;
}

