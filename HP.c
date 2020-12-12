#include "HP.h"
#include "Record.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// /* Use to store needed information for heap file when HP_OpenFile() is used*/
// typedef struct{
// 	int fileDesc;	// id returned by BF_OpenFile() to identify said file
// 	char attrType;	// type that describes key-field ('c'or 'i')
// 	char* attrName;	// name of field that is key
// 	int attrLength;	// size of key-field
// }HP_info;


/* Creates and properly initializes a heap file called "filename" */
/* Returns: 0 for success, -1 for failure */
int HP_CreateFile(	char *filename,
			char attrType,	// key type 'c' or 'i'
			char *attrName,	// name of field that is key
			int attrLength	// size of key-field
		 )
{
	int fileDesc;
	void *block;
	if (attrType != 'i' && attrType != 'c'){
		printf("Error illegal character.\n");
		return -1;
	}
									// ADD ERROR CHECKS!!!
	BF_CreateFile(filename);
	
 	BF_OpenFile(filename);
 	fileDesc = BF_AllocateBlock(filename);
 	BF_ReadBlock(fileDesc, 0, &block);
	memcpy(block, &fileDesc, sizeof(int));
	block += sizeof(int);

	memcpy(block, &attrType, sizeof(char));
	block += sizeof(char);
	
	memcpy(block, &attrType, strlen(attrName)+1)
	block += strlen(attrName);
	
	memcpy(block, attrLength);
	BF_CloseFile(fileDesc);
}

/* Open file named "filename" and reads the info relevant to heap file from first block */
/* In case of an error, returns NULL */
HP_Info* HP_OpenFile(	char *filename)
{
	
}

/* Close file given by said structure */
/* Returns: 0 for success, -1 for failure*/
/* When successfully closed file -> deallocate memory used up by the structure*/
int HP_CloseFile(	HP_info *header_info)
{
	
}

/* Inserts record into a block of the file identified by HP_info */
/* Returns: blockId in which record was inserted upon success, -1 upon failure*/
int HP_InsertEntry(	HP_info header_info,
			Record record;	// structure in Record.h representing a record that gets stored
		  )
{
	
}

/* Deletes record with primary key 'value' */
/* Returns: 0 upon success, -1 upon failure */
int HP_DeleteEntry(	HP_info header_info,
			void *value // primary key value
		  )
{
	
}

/* Find and print all entries with primary key 'value' */
/* Returns: number of blocks read upon success, -1 upon failure*/
int HP_GetAllEntries(	HP_info header_info,
			void *value // primary key value
		    )
{
	
}
