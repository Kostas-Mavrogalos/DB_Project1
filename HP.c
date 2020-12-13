#include "HP.h"
#include "Record.h"
#include "BF.h"
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
int HP_CreateFile(char *filename, char attrType, char *attrName, int attrLength)
{
	int fileDesc;
	void *block;
	if (attrType != 'i' && attrType != 'c'){
		printf("Error illegal character");
		return -1;
	}

	if (BF_CreateFile(filename) == -1 ) {
		BF_PrintError("Couldn't create file");
	  return -1;
	}

  	if ( (fileDesc = BF_OpenFile(filename)) < 0 ) {
		BF_PrintError("Couldn't open file");
		return -1;
	}

 	if (BF_AllocateBlock(fileDesc) < 0 ) {
		BF_PrintError("Couldn't allocate block");
		return -1;
	}

 	if (BF_ReadBlock(fileDesc, 0, &block) < 0 ) {
		BF_PrintError("Couldn't read block");
		return -1;
	}

	if (BF_WriteBlock(fileDesc, 0) < 0){
		BF_PrintError("Couldnt' write block");
		return -1;
	}
	
	memcpy(block, &fileDesc, sizeof(int));
	block += sizeof(int);

	memcpy(block, &attrType, sizeof(char));
	block += sizeof(char);

	strcpy(block, attrName);
	block += strlen(attrName) + 1;

	memcpy(block, &attrLength, sizeof(int));

																							//BF_Write(fileDesc, )

	if (BF_CloseFile(fileDesc)< 0 ) {
		BF_PrintError("Couldn't close file");
		return -1;
	}

	return 0;
}

/* Open file named "filename" and reads the info relevant to heap file from first block */
/* In case of success, initializes HP_info an error, returns NULL */
HP_info* HP_OpenFile(char *filename)
{
	int fileDesc;
	void* block;
	if ( (fileDesc = BF_OpenFile(filename)) < 0 ) {
		BF_PrintError("Couldn't open file");
	  return NULL;
	}

	if (BF_ReadBlock(fileDesc, 0, &block) < 0 ) {
		BF_PrintError("Couldn't read file");
		return NULL;
	}

	HP_info* header_info = malloc(sizeof(HP_info));


	memcpy(&(header_info->fileDesc), block, sizeof(int));
	block+=sizeof(int);

	memcpy(&(header_info->attrType), block, sizeof(char));
	block+=sizeof(char);

	strcpy(header_info->attrName, block);
	block+=strlen(block) + 1;

	memcpy(&(header_info->attrLength), block, sizeof(int));

	return header_info;
}

/* Close file given by said structure */
/* Returns: 0 for success, -1 for failure*/
/* When successfully closed file -> deallocate memory used up by the structure*/
int HP_CloseFile(HP_info *header_info)
{
	if (BF_CloseFile(header_info->fileDesc) < 0) {
		BF_PrintError("Couldn't close file")l
		return -1;
	}

	free(header_info);

	return 0;
}

/* Inserts record into a block of the file identified by HP_info */
/* Returns: blockId in which record was inserted upon success, -1 upon failure*/
int HP_InsertEntry(HP_info header_info, Record record)
{

	int block_number;
	int num_of_records;
	void *block;
	void *num_records_p;
	void *next_block_p;
	Record *first_available;
	
	// block_number is the index of the last block, where the insertion is attempted.
	block_number = BF_GetBlockCounter(header_info->fileDesc) - 1;
	
	if (BF_ReadBlock(header_info->fileDesc, block_number, &block) < 0){
		BF_PrintError("Couldn't read block");
		return -1;
	}
	
	// If the current last block is 0, allocate a new one. Never insert a record in the first block.
	if(block_number-1 == 0){
		if (BF_AllocateBlock(fileDesc) < 0 ) {
			BF_PrintError("Couldn't allocate block");
			return -1;
		}
		if (BF_ReadBlock(fileDesc, block_number, &block) < 0 ) {
			BF_PrintError("Couldn't read block");
			return -1;
		}
		
		if (BF_WriteBlock(fileDesc, block_number) < 0 ) {
			BF_PrintError("Couldnt' write block");
			return -1;
		}
		
		memcpy(BLOCK_SIZE - 1 - sizeof(int*), block, sizeof(int*));
	}
	
	// Go to the last 8 bytes and store the number of records
	num_records_p = block + ((BLOCK_SIZE -1) - sizeof(int) + 1);
	memcpy(&num_of_records, num_records_p, sizeof(int));
	// Right before the bytes storing num_of_record resides the pointer to the next block.
	next_block_p = num_records_p - sizeof(int*);
	
	// Move the pointer for insertion sizeof(Record*) times the records inserted.
	first_available = (Record *)(block + (num_of_records+1)*sizeof(Record*));
	
	// If there is enough space in this block, store the record.
	if (next_block_p - (void *)first_available >= sizeof(Record*)) {
		memcpy(first_available, &record, sizeof(Record*));
		num_of_records ++;
		memcpy(num_records_p, &num_of_records, sizeof(int));
		return block_number;
	}
	
	// If the block is full, allocate memory for a new one and insert the record at the beginning.
	block_number ++;
	if (BF_AllocateBlock(fileDesc) < 0 ) {
		BF_PrintError("Couldn't allocate block");
		return -1;
	}
	if (BF_ReadBlock(fileDesc, block_number, &block) < 0 ) {
		BF_PrintError("Couldn't read block");
		return -1;
	}
		
	if (BF_WriteBlock(fileDesc, block_number) < 0 ) {
		BF_PrintError("Couldnt' write block");
		return -1;
	}
	
	memcpy(next_block_p, block, sizeof(int*));
	
	num_records_p = block + ((BLOCK_SIZE -1) - sizeof(int) + 1);
	memcpy(block, &record, sizeof(Record*));
	num_of_records = 1;
	memcpy(num_records_p, &num_of_records, sizeof(int));
	return block_number;
	
}

/* Deletes record with primary key 'value' */
/* Returns: 0 upon success, -1 upon failure */
int HP_DeleteEntry(	HP_info header_info, void *value)
{

}

/* Find and print all entries with primary key 'value' */
/* Returns: number of blocks read upon success, -1 upon failure*/
int HP_GetAllEntries(	HP_info header_info,
			void *value // primary key value
		    )
{

}
