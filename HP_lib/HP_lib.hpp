


typedef struct{
	int id;
	char name[15];
	char surname[25];
	char address[50];
}Record; 

typedef struct{
	std :: string HP_filename;
	int fileDesc;
	char attrType;
    char* attrName;
    int attrLength;

}HP_info;



typedef struct 
{
	int cap;
	Record arr[5]; 
	
}block_node;


int HP_CreateFile( char *fileName,char attrType,char* attrName, int attrLength );

HP_info* HP_OpenFile(char *fileName);

int HP_CloseFile(HP_info* header_info);

int HP_InsertEntry( HP_info header_info,Record record );

int Print_All_Records( HP_info header_info);
