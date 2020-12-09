#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
#include "HP_lib.hpp"
using namespace std;



int main(void)
{
/*
std :: string HP_filename;
	int fileDesc;
	char attrType;
    char* attrName;
    int attrLength;

	char *k;
	HP_info* info;
	char p= 'i';
	k=new char[15];
	strcpy(k,"ok");
	HP_CreateFile(k,p,k,5);


	info=HP_OpenFile(k);

//printing the info of the file
	if(info !=NULL)
	{
		cout << "file's  Name : " <<info->HP_filename<< endl;
		cout << "file's  fileDesc : " <<info->fileDesc<< endl;
		cout << "file's  attrType : " <<info->attrType<< endl;
		cout << "file's  attrName : " <<info->attrName<< endl;
		cout << "file's  attrLength : " <<info->attrLength<< endl;
	}

	int integer=69;

//reading and inserting Records for a specific file
	Read_From_File(*info,"records1K.txt");

//geting Entries with id 69 - 72
	HP_GetAllEntries(*info,&integer);
	integer++;
	HP_GetAllEntries(*info,&integer);
	integer++;
	HP_GetAllEntries(*info,&integer);
	integer++;
	HP_GetAllEntries(*info,&integer);

	HP_DeleteEntry(*info,&integer);

	HP_DeleteEntry(*info,&integer);


	//Print_All_Records(*info);
	HP_CloseFile(info);

	


	delete[] k;
	return 0;
	*/

	if (HP_CreateFile("HeapFile1",'i',"Attribute",5) == 0){
        cout << "File created sucessfully!" << endl;
    }

    HP_info* HP1_info = HP_OpenFile("HeapFile1");

    Read_From_File(*HP1_info,"records5K.txt");


    cout << "----------------------------- DELETE TEST -----------------------------" << endl;

	int valuesToDelete[4] = {150,12,2400,54445};
    if (HP_DeleteEntry(*HP1_info, &valuesToDelete[0]) != -1) cout << "Record with id " << valuesToDelete[0] << " deleted successfully!" << endl;
    else cout<< "Error deleting record with id " << valuesToDelete[0] << endl;

    if (HP_DeleteEntry(*HP1_info, &valuesToDelete[1]) != -1) cout << "Record with id " << valuesToDelete[1] << " deleted successfully!" << endl;
    else cout<< "Error deleting record with id " << valuesToDelete[1] << endl;

    if (HP_DeleteEntry(*HP1_info, &valuesToDelete[2]) != -1) cout << "Record with id " << valuesToDelete[2] << " deleted successfully!" << endl;
    else cout<< "Error deleting record with id " << valuesToDelete[2] << endl;

    //Delete already deleted entry
    if (HP_DeleteEntry(*HP1_info, &valuesToDelete[2]) != -1) cout << "Record with id " << valuesToDelete[2] << " deleted successfully!" << endl;
    else cout<< "Error deleting record with id " << valuesToDelete[2] << endl; 
    
    //Delete non-existent entry
    if (HP_DeleteEntry(*HP1_info, &valuesToDelete[3]) != -1) cout << "Record with id " << valuesToDelete[3] << " deleted successfully!" << endl;
    else cout<< "Error deleting record with id " << valuesToDelete[3] << endl;   


    cout << "----------------------------- SEARCH TEST -----------------------------" << endl;

    int valuesToSearch[3] = {500,150,99999};
    
    if (HP_GetAllEntries(*HP1_info, &valuesToSearch[0]) != -1) cout << "Record with id " << valuesToSearch[0] << " found!" << endl;
    else cout<< "Record with id " << valuesToSearch[0] << " not found!" << endl;   
    
    //Search deleted entry
    if (HP_GetAllEntries(*HP1_info, &valuesToSearch[1]) != -1) cout << "Record with id " << valuesToSearch[1] << " found!" << endl;
    else cout<< "Record with id " << valuesToSearch[1] << " not found!" << endl;     

    //Search non-exitent entry
    if (HP_GetAllEntries(*HP1_info, &valuesToSearch[2]) != -1) cout << "Record with id " << valuesToSearch[2] << " found!" << endl;
    else cout<< "Record with id " << valuesToSearch[2] << " not found!" << endl;    

    //-------------------------------------------------------------------------------------------------------------------------------//


    HP_CloseFile(HP1_info);

    return 0;



}
