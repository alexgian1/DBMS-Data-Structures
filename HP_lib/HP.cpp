  
#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
#include "HP_lib.hpp"
extern "C"{  //link with C library
    #include "../BF_lib/BF.h"  
}

using namespace std;


int main(void)
{
	char *k;
	HP_info* info;
	char p= 'i';
	k=new char[15];
	strcpy(k,"ok");
	HP_CreateFile(k,p,k,300);

	info=HP_OpenFile(k);
	void *del;




	Read_From_File(*info);
	Print_All_Records(*info);

	HP_CloseFile(info);

	


	delete[] k;

	return 0;
}




int HP_CreateFile( char *fileName,char attrType,char* attrName, int attrLength )
{
	int filenum;

	string string_filename(fileName);

	BF_Init();
	if (BF_CreateFile(fileName) < 0) {
		BF_PrintError("Error creating file");
		exit(EXIT_FAILURE);
	}
	if ((filenum = BF_OpenFile(fileName)) < 0) {
		BF_PrintError("Error opening file");
		return -1;
	}

	if (BF_AllocateBlock(filenum) < 0) {
		BF_PrintError("Error allocating block");
		return -1;
	}
	HP_info* info= new HP_info{string_filename,filenum,attrType,attrName,attrLength};



	void * block;
	if (BF_ReadBlock(filenum, 0, &block) < 0) {
		BF_PrintError("Error getting block");
		return -1;
	}

	memcpy(block,info,sizeof(HP_info));



	block_node *node= new block_node;

	void * block2;
	if (BF_AllocateBlock(filenum) < 0)
			return -1;

		if (BF_ReadBlock(filenum, 1, &block2) < 0) {
			BF_PrintError("Error getting block");
			return -1;
		}
		node->cap=0;
	memcpy(block2,node,sizeof(block_node));

	


	if (BF_WriteBlock(filenum,0) < 0) return -1;

	if (BF_CloseFile(filenum) < 0) {
		BF_PrintError("Error closing file");
		return -1;
	}
	//delete info;
	return 0;
} 





HP_info* HP_OpenFile(char *fileName) 
{

	HP_info* info_block = new HP_info;
	int filenum;

	if ((filenum = BF_OpenFile(fileName)) < 0) {
		BF_PrintError("Error opening file");
		return NULL;
	}

	void * block;
	if (BF_ReadBlock(filenum, 0, &block) < 0) {
		BF_PrintError("Error getting block");
		return NULL;
	}

	memcpy(info_block,block,sizeof(HP_info));

	//cout << info_block->HP_filename<<" name: " <<info_block->fileDesc<< " surname: "<< info_block->attrType << " address: "<< info_block->attrName <<"<-------"<<endl;
	return info_block;
}


int HP_CloseFile( HP_info* header_info )
{
	if (BF_CloseFile(header_info->fileDesc) < 0) {
		BF_PrintError("Error closing file");
		return -1;
	}
	
	return 0;

}



int HP_InsertEntry( HP_info header_info , Record record )
{

	int file_code=header_info.fileDesc;
	void* block;
	block_node *this_block= new block_node;


	int num_of_blocks = BF_GetBlockCounter(file_code);

	for(int Block=1 ; Block < num_of_blocks ; Block++ )
	{
		if (BF_ReadBlock(file_code, Block, &block) < 0) {
				BF_PrintError("Error getting block");
				return -1;
			}

		memcpy(this_block,block,sizeof(block_node));

		int num_of_recs=this_block->cap;

		if(this_block->del == 1)
		{
				for(int p=0; p < num_of_recs; p++)
				{
					if(this_block->arr[p].id == -1)
					{
						num_of_recs=p;
						this_block->arr[num_of_recs] = record;
						memcpy(block,this_block,sizeof(block_node));
						if (BF_WriteBlock(file_code,Block) < 0) return -1;
						delete this_block;
						return Block;
					}
				}
		}

		if(num_of_recs < 5)
		{
			
			this_block->arr[num_of_recs] = record;
			this_block->cap+=1;
			memcpy(block,this_block,sizeof(block_node));
			if (BF_WriteBlock(file_code,Block) < 0) return -1;
			delete this_block;
			return Block;
		}
		if(Block == (num_of_blocks - 1))
		{
			if (BF_AllocateBlock(file_code) < 0) return -1;

			if (BF_ReadBlock(file_code, Block+1, &block) < 0) return -1;

			this_block->arr[0] = record;
			this_block->cap = 1;
			this_block->del=0;
			memcpy(block,this_block,sizeof(block_node));

			if (BF_WriteBlock(file_code,Block+1) < 0) return -1;
			delete this_block;
			return Block+1;
		}
		else
			continue;
	}
	delete this_block;
	return -1;
}

int Print_All_Records( HP_info header_info)
{

	int file_code=header_info.fileDesc;
	int num_of_blocks = BF_GetBlockCounter(file_code);
	void* block;
	block_node node2;

	for(int Block=1 ; Block < num_of_blocks ; Block++ )
		{

			if (BF_ReadBlock(file_code, Block, &block) < 0) {
				BF_PrintError("Error getting block");
				return -1;
			}
			cout << endl<<endl;
			memcpy(&node2,block,sizeof(block_node));

			cout << "Block "<<Block << "with "<< node2.cap << " Records :"<<endl;
			for (int rec=0;rec < node2.cap; rec++)
			{
				cout << " Record with : name=" <<node2.arr[rec].name << " "<< node2.arr[rec].surname << " And address : " << node2.arr[rec].address <<" and id:"<<node2.arr[rec].id <<endl;
			}

		}
		return 0;


}



int HP_DeleteEntry(HP_info header_info,int this_id) 
{
	//int *this_idd=static_cast<int*>(value);
	//int this_id = *this_idd;


	int file_code=header_info.fileDesc;
	int num_of_blocks = BF_GetBlockCounter(file_code);
	void* block;
	block_node *node = new block_node;

	for(int Block=1 ; Block < num_of_blocks ; Block++ )
		{

			if (BF_ReadBlock(file_code, Block, &block) < 0) {
				BF_PrintError("Error getting block");
				return -1;
			}
			memcpy(node,block,sizeof(block_node));

			for (int rec=0;rec < node->cap; rec++)
			{
				if(node->arr[rec].id == this_id)
				{
					node->arr[rec].id = -1;
					strcpy(node->arr[rec].name,"DELETED");
					strcpy(node->arr[rec].surname,"DELETED");
					strcpy(node->arr[rec].address,"DELETED");
					node->del=1;
					memcpy(block,node,sizeof(block_node));

					if (BF_WriteBlock(file_code,Block+1) < 0) return -1;
					break;
				}

			}

		}
		delete node;
		return 0;
}


void Read_From_File(HP_info header_info)
{


	Record rec;


	ifstream myReadFile;
 	myReadFile.open("../record_examples/records5K.txt");
 	string output;
 	if (myReadFile.is_open()) {

 		while (!myReadFile.eof()) {


    		myReadFile >> output;
    		//cout<<output<<endl;

    		std::size_t pos = output.find(",");
    		std::string id = output.substr (1,pos);
    		//cout<<"id=" <<id<<endl;

    		std::string remaining = output.substr (pos+2);
    		std::size_t pos2 = remaining.find(",");
    		pos2--;
    		std::string name = output.substr (pos+2 ,pos2);
    		//cout<<"name=" <<name<<endl;


    		remaining = remaining.substr (pos2+3);
    		std::size_t pos3 = remaining.find(",");
    		pos3--;
    		std::string surname = remaining.substr (0 ,pos3);
    		//cout<<"surname=" <<surname<<endl;


    		remaining = remaining.substr (pos3+3);
    		std::size_t pos4 = remaining.find("}");
    		pos3--;
    		std::string address = remaining.substr (0 ,pos4-1);
    		//cout<<"address= " <<address<<endl;

    		int int_id= stoi(id);

    		rec.id=int_id;
    		strcpy(rec.name,name.c_str());
    		strcpy(rec.surname,surname.c_str());
    		strcpy(rec.address,address.c_str());


    		HP_InsertEntry(header_info,rec);

 		}
	}
	myReadFile.close();
}