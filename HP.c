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

	memcpy(block, &fileDesc, sizeof(int));
	block += sizeof(int);

	memcpy(block, &attrType, sizeof(char));
	block += sizeof(char);

	strcpy(block, attrName);
	block += strlen(attrName) + 1;

	memcpy(block, &attrLength, sizeof(int));

	if (BF_WriteBlock(fileDesc, 0) < 0){
		BF_PrintError("Couldnt' write block");
		return -1;
	}

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
		BF_PrintError("Couldn't close file");
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
	int *num_records_p;
	int *next_block_p;
	void* first_available;

	// block_number is the index of the last block, where the insertion is attempted.
	block_number = BF_GetBlockCounter(header_info.fileDesc) - 1;

	if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0){
		BF_PrintError("Couldn't read block");
		return -1;
	}

	// If the current last block is 0, allocate a new one. Never insert a record in the first block.
	if (block_number == 0) {
		if (BF_AllocateBlock(header_info.fileDesc) < 0 ) {
			BF_PrintError("Couldn't allocate block");
			return -1;
		}

		block_number = BF_GetBlockCounter(header_info.fileDesc) - 1;
		next_block_p = block;
		next_block_p += BLOCK_SIZE - sizeof(int);

		if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0 ) {
			BF_PrintError("Couldn't read block");
			return -1;
		}

		memcpy(next_block_p, &block_number, sizeof(int));															//Write the next block * to the end of the first block

		if (BF_WriteBlock(header_info.fileDesc, block_number) < 0 ) {
			BF_PrintError("Couldn't write block");
			return -1;
		}
	}

	// Go to the last 4 bytes and store the number of records
	num_records_p = block;
	num_records_p += BLOCK_SIZE - sizeof(int);
	memcpy(&num_of_records, num_records_p, sizeof(int));						// Get the number of records
	// Right before the bytes storing num_of_record resides the pointer to the next block.
	next_block_p = num_records_p - sizeof(int);

	// Move the pointer for insertion sizeof(Record*) times the records inserted.
	first_available = (Record *)(block + num_of_records*sizeof(Record));

	// If there is enough space in this block, store the record.
	if (next_block_p - (void *)first_available >= sizeof(Record)) {
		memcpy(first_available, &record, sizeof(Record));

		num_of_records++;
		printf("%d records in block# %d for id %d\n",num_of_records, block_number, record.id);
		memcpy(num_records_p, &num_of_records, sizeof(int));

		if (BF_WriteBlock(header_info.fileDesc, block_number) < 0 ) {
			BF_PrintError("Couldn't write block");
			return -1;
		}

		return block_number;
	}

	// If the block is full, allocate memory for a new one and insert the record at the beginning.
	block_number++;
	if (BF_AllocateBlock(header_info.fileDesc) < 0 ) {
		BF_PrintError("Couldn't allocate block");
		return -1;
	}
	if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0 ) {
		BF_PrintError("Couldn't read block");
		return -1;
	}

	memcpy(next_block_p, &block_number, sizeof(int));

	num_records_p = block;
	num_records_p += BLOCK_SIZE - sizeof(int);
	memcpy((Record*)block, &record, sizeof(Record));
	num_of_records = 1;
	memcpy(num_records_p, &num_of_records, sizeof(int));

	if (BF_WriteBlock(header_info.fileDesc, block_number) < 0 ) {
		BF_PrintError("Couldn't write block");
		return -1;
	}

	return block_number;
}

/* Deletes record with primary key 'value' */
/* Returns: 0 upon success, -1 upon failure */
int HP_DeleteEntry(HP_info header_info, void *value)
{
	int block_number;
	void* block;
	void* next_block_p;
	int key_size = header_info.attrLength;
	Record* record;							//To clear the Record that is to be deleted
	Record read;

	block_number = BF_GetBlockCounter(header_info.fileDesc);

	if (block_number == 1) return -1;

	if (BF_ReadBlock(header_info.fileDesc, 1, &block) < 0){				//Read the block that has data
		BF_PrintError("Couldn't read block");
		return -1;
	}

	//next_block_p points to the next block
	next_block_p = block;
	next_block_p += BLOCK_SIZE - 2*sizeof(int);

	//record now points to the first (key) # of bytes of the block, where the primary key value of the Record struct is stored
	record = block;
	memcpy(&read, record, sizeof(Record));

	while (memcmp(&read.id, value, key_size) != 0) {
		if (next_block_p - (void*)record < sizeof(Record)) {					//For when the available space in the block isn't enough for a Record to fit
			if (next_block_p == NULL) {						//If there isn't a next block
				return -1;
			}
			memcpy(&block_number, next_block_p, sizeof(int));
			if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0){
				BF_PrintError("Couldn't read block");
				return -1;
			}
			record = block;
			memcpy(&read, record, sizeof(Record));

			//if (memcmp(read.id, value, key_size) == 0) break;		//When record changes, we need to see the first Record's id, and if it's equal to value, exit the loop

			next_block_p = block;
			next_block_p += BLOCK_SIZE - 2*sizeof(int);
		}else{
			//If the value in that record isn't the one we are looking for, move Record # of bytes forward
			record += sizeof(Record);
		}
	}

	//Empty the value and fill it with 0's
	Record del;
	memset(record, 0, sizeof(Record));
	memcpy(&read, record, sizeof(Record));
	printf("id: %d\n", read.id);
	return 0;
}


/* Find and print all entries with primary key 'value' */
/* Returns: number of blocks read upon success, -1 upon failure*/
int HP_GetAllEntries(HP_info header_info, void *value)
{
	int val = (int)value;
	int block_number = 0;
	int all;														//pseudo-boolean integer to know if we will print all or 1 Entry
	void* block;
	int* next_block_p;
	int key_size = header_info.attrLength;
	Record* read;
	Record record;
	Record empty_record;

	memset(empty_record, 0, sizeof(Record));

// 	if (!value) {
// 		all = 1;
// 		printf("sasasas\n");
// 	} else {
// 		val = *(int*)value;
// 		printf("%d VAL\n", val);
// 		all = 0;
// 	}
	
	// First, go to block 0.
	if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0){
		BF_PrintError("Couldn't read block");
		return -1;
	}
	
	// Then, using the index stored at the block's last bytes, go to the next block (1).
	block += BLOCK_SIZE - sizeof(int);
	memcpy(&block_number, (int*)block, sizeof(int));
	if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0){
		BF_PrintError("Couldn't read block");
		return -1;
	}

	//next_block_p points to the next block
	next_block_p = (int*)block;
	next_block_p += BLOCK_SIZE - 2*sizeof(int);

	read = (Record*)block; 
	memcpy(&record, read, sizeof(Record));

	//read now points to the first (key) # of bytes of the block, where the primary key value of the Record struct is stored
	//memcpy(read, block, sizeof(Record));

	//if (all == 0) {
		while (memcmp(&record.id, value, key_size) != 0) {
			printf("%d READ ID\n",record.id);
			printf("%s READ NAME\n", record.name);
			// If the end of a block is reached, move the block pointer to the next block, if there is one
			if (next_block_p - (void*)read < sizeof(Record)) {
				// If end of file reached, no valid key was given
				if (next_block_p == NULL) {
					return -1;
				}
				memcpy(&block_number, next_block_p, sizeof(int));
				if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0){
					BF_PrintError("Couldn't read block");
					return -1;
				}

				read = (Record*)block;
				memcpy(&record, read, sizeof(Record));

				if (record.id == val) break;		//When record changes, we need to see the first Record's id, and if it's equal to value, exit the loop

				next_block_p = block;
				next_block_p += BLOCK_SIZE - 2*sizeof(int);
			}

			//If the value in that record isn't the one we are looking for, move Record # of bytes forward
			read += sizeof(Record);
			memcpy(&record, read, sizeof(Record));
		}
			//Print all the info of record with queried id
			printf("Found record with id: %d\n", record.id);
			printf("This record's name is: %s\n", record.name);
			printf("This record's surname is: %s\n", record.surname);
			printf("This record's address is: %s\n", record.address);
			return block_number;
	//}

// 	if (all == 1) {

// 		while (next_block_p - (void*)read >= sizeof(Record) && next_block_p != NULL) {
// 			printf("SEX21\n");
// 			 if (next_block_p - (void*)read < sizeof(Record)) {					//For when the available space in the block isn't enough for a Record to fit
// 				memcpy(&block_number, next_block_p, sizeof(int));
// 				if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0){
// 					BF_PrintError("Couldn't read block");
// 					return -1;
// 				}

// 				memcpy(read, block, key_size);
// 				memcpy(&record, read, sizeof(Record));							//Whenever we change blocks, record must change in order to print the next Record

// 				next_block_p = block;
// 				next_block_p += BLOCK_SIZE - 2*sizeof(int);
// 			}

// 			memcpy(&record, read, sizeof(Record));

// 			// //If the record isn't NULL, meaning it has stuff in it, print them
// 			// if (memcmp(&record, test, sizeof(Record)) != 0) {
// 			// 	printf("This record's id is: %d\n", record.id);
// 			// 	printf("This record's name is: %s\n", record.name);
// 			// 	printf("This record's surname is: %s\n", record.surname);
// 			// 	printf("This record's address is: %s\n\n", record.address);					//Print all the info of the record
// 			// }

// 			//Move the variable Record # of bytes forward
// 			read += sizeof(Record);
// 		}

// 		return block_number;
// 	}

// 	return -1;
}

// int HP_GetAllEntries(HP_info header_info, void *value){
// 	int block_number = 1;
// 	void* block;
// 	if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0 ) {
// 		BF_PrintError("Couldn't read block");
// 		return -1;
// 	}
// 	Record rec;
// 	Record* read;
// 	read = block;
// 	memcpy(&rec, read, sizeof(Record));
// 	printf("ID: %d", rec.id);
// 	printf("name: %s", rec.name);
// 	printf("surname: %s", rec.surname);
// }	
