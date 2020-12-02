  
#include <iostream>
#include <cstring>
#include <fstream>
#include "HP_lib.hpp"
extern "C"{  //link with C library
    #include "../BF_lib/BF.h"  
}

using namespace std;


int main(void)
{

	Record Data;
	Data.id=1;
	strcpy(Data.name," George");
	strcpy(Data.surname,"Papa");
	strcpy(Data.address,"Patra");


	Record Dataa;
	Dataa.id=2;
	strcpy(Dataa.name," John");
	strcpy(Dataa.surname,"Xac");
	strcpy(Dataa.address,"Ath");


	Record Dataaa;
	Dataaa.id=3;
	strcpy(Dataaa.name," Lissa");
	strcpy(Dataaa.surname,"Jas");
	strcpy(Dataaa.address,"Cal");

	Record Dataaaa;
	Dataaaa.id=4;
	strcpy(Dataaaa.name," Jull");
	strcpy(Dataaaa.surname,"Bl");
	strcpy(Dataaaa.address,"Sw");
//----
	Record Dataaaaa;
	Dataaaaa.id=5;
	strcpy(Dataaaaa.name," Maria");
	strcpy(Dataaaaa.surname,"Kal");
	strcpy(Dataaaaa.address,"ios");

	Record Dataaaaaa;
	Dataaaaaa.id=6;
	strcpy(Dataaaaaa.name," danai");
	strcpy(Dataaaaaa.surname,"pal");
	strcpy(Dataaaaaa.address,"Paros");

	Record Dataaaaaaa;
	Dataaaaaaa.id=7;
	strcpy(Dataaaaaaa.name," Nicolle");
	strcpy(Dataaaaaaa.surname,"Ded");
	strcpy(Dataaaaaaa.address,"Thisio");

	Record Dataaaaaaaa;
	Dataaaaaaaa.id=8;
	strcpy(Dataaaaaaaa.name," Lef");
	strcpy(Dataaaaaaaa.surname,"Vam");
	strcpy(Dataaaaaaaa.address,"Pli");

	Record Dataaaaaaaaa;
	Dataaaaaaaaa.id=9;
	strcpy(Dataaaaaaaaa.name," Max");
	strcpy(Dataaaaaaaaa.surname,"Tow");
	strcpy(Dataaaaaaaaa.address,"Us");

	Record Dataaaaaaaaaa;
	Dataaaaaaaaaa.id=10;
	strcpy(Dataaaaaaaaaa.name," Ow");
	strcpy(Dataaaaaaaaaa.surname,"Co");
	strcpy(Dataaaaaaaaaa.address,"Cal");


	char *k;
	HP_info* info;
	char p= 'i';
	k=new char[15];
	strcpy(k,"ok");
	HP_CreateFile(k,p,k,300);

	info=HP_OpenFile(k);

/*
for (int i = 1; i < 100; i++)
{

		block_node *node= new block_node;

	void * block2;
	if (BF_AllocateBlock(info->fileDesc) < 0)
			return -1;

		if (BF_ReadBlock(info->fileDesc, i, &block2) < 0) {
			BF_PrintError("Error getting block");
			return -1;
		}
		node->cap=0;
		memcpy(block2,node,sizeof(block_node));


	if (BF_WriteBlock(info->fileDesc,i) < 0) return -1;
	delete node;

}
*/

	int succ;
	for(int ii= 1 ; ii<=150 ;  ii++){
	succ=HP_InsertEntry(*info,Data);
	succ=HP_InsertEntry(*info,Dataa);
	succ=HP_InsertEntry(*info,Dataaa);
	succ=HP_InsertEntry(*info,Dataaaa);
	succ=HP_InsertEntry(*info,Dataaaaa);
	succ=HP_InsertEntry(*info,Dataaaaaa);
	succ=HP_InsertEntry(*info,Dataaaaaaa);
	succ=HP_InsertEntry(*info,Dataaaaaaaa);
	succ=HP_InsertEntry(*info,Dataaaaaaaaa);
	succ=HP_InsertEntry(*info,Dataaaaaaaaaa);

}





	
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

	cout << info_block->HP_filename<<" name: " <<info_block->fileDesc<< " surname: "<< info_block->attrType << " address: "<< info_block->attrName <<"<-------"<<endl;
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

	cout << "----->>>" <<num_of_blocks<<endl;
	for(int Block=1 ; Block < num_of_blocks ; Block++ )
	{
		if (BF_ReadBlock(file_code, Block, &block) < 0) {
				BF_PrintError("Error getting block");
				return -1;
			}

		memcpy(this_block,block,sizeof(block_node));

		int num_of_recs=this_block->cap;

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
			if (BF_AllocateBlock(file_code) < 0){
				cout << "ERRRRROOOORRR"<<endl;
				return -1;
			}

			if (BF_ReadBlock(file_code, Block+1, &block) < 0) {
				BF_PrintError("Error getting block");
				return -1;
			}
			this_block->arr[0] = record;
			this_block->cap = 1;
			memcpy(block,this_block,sizeof(block_node));

			if (BF_WriteBlock(file_code,Block+1) < 0) return -1;
			delete this_block;
			return Block+1;
		}
		else
			continue;
	}
	delete this_block;
	cout << "=============================================================="<< endl;
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


