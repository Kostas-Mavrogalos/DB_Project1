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
	void *num_records_p;
	void *next_block_p;
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

		block_number = BF_GetBlockCounter(header_info.fileDesc) - 1;			//Same as block_number++
		next_block_p = block;
		next_block_p += BLOCK_SIZE - sizeof(int);					//Go to the memory where the block ID for the next block is stored


		if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0 ) {
			BF_PrintError("Couldn't read block");
			return -1;
		}

		memcpy(next_block_p, &block_number, sizeof(int));															//Write the next block ID to the end of the first block

		if (BF_WriteBlock(header_info.fileDesc, block_number) < 0 ) {
			BF_PrintError("Couldn't write block");
			return -1;
		}
	}

	// Go to the last 4 bytes and store the number of records
	num_records_p = block;
	num_records_p += BLOCK_SIZE - sizeof(int);
	memcpy(&num_of_records, num_records_p, sizeof(int));						// Get the number of records


	printf("%d NUM OF RECORDS, %d RECORD\n", num_of_records, record.id);
	// Right before the bytes storing num_of_record resides the pointer to the next block.
	next_block_p = num_records_p - sizeof(int);


	// Move the pointer for insertion sizeof(Record*) times the records inserted.
	first_available = (Record *)(block + num_of_records*sizeof(Record));

	// If there is enough space in this block, store the record.
	if ((void*)next_block_p - (void *)first_available >= sizeof(Record)) {
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
	memcpy(block, &record, sizeof(Record));

	memcpy(next_block_p, &block_number, sizeof(int));

	num_records_p = block;
	num_records_p += BLOCK_SIZE - sizeof(int);
	memcpy((Record*)block, &record, sizeof(Record));
	num_of_records = 1;
	memcpy(num_records_p, &num_of_records, sizeof(int));
	printf("%d records in block# %d for id %d\n",num_of_records, block_number, record.id);
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
	void* record;							//To clear the Record that is to be deleted
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
	memcpy(&read, (Record*)record, sizeof(Record));

	while (1) {
		printf("ID: %d\n", read.id);
		if (memcmp(&read.id, (int*)value, key_size) == 0){
			//Empty the value and fill it with 0's
			Record del;
			memset(record, 0, sizeof(Record));
			memcpy(&read, record, sizeof(Record));
			printf("id: %d\n", read.id);
			BF_WriteBlock(header_info.fileDesc, block_number);
			return 0;
		}
		printf("Konnichiwa\n");
		if (next_block_p - (void*)record < sizeof(Record)) {					//For when the available space in the block isn't enough for a Record to fit
			if (memcmp(next_block_p, (char[sizeof(int)]){0}, sizeof(int))==0) {						//If there isn't a next block
				return -1;
			}
			memcpy(&block_number, next_block_p, sizeof(int));
			if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0){
				BF_PrintError("Couldn't read block");
				return -1;
			}
			record = block;
			memcpy(&read, (Record*)record, sizeof(Record));

			//if (memcmp(read.id, value, key_size) == 0) break;		//When record changes, we need to see the first Record's id, and if it's equal to value, exit the loop

			next_block_p = (int*)block;
			next_block_p += BLOCK_SIZE - 2*sizeof(int);
		}else{
			//If the value in that record isn't the one we are looking for, move Record # of bytes forward
			record += sizeof(Record);
			memcpy(&read, (Record*)record, sizeof(Record));
		}
	}

}


/* Find and print all entries with primary key 'value' */
/* Returns: number of blocks read upon success, -1 upon failure*/
int HP_GetAllEntries(HP_info header_info, void *value)
{
	int block_number = 1;
	int all;														//pseudo-boolean integer to know if we will print all or 1 Entry
	void* block;
	void* next_block_p;
	int key_size = header_info.attrLength;
	void* read;
	Record record;
	Record empty_record;

	memset(&empty_record, 0, sizeof(Record));

	// First, go to block 0.
	if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0){
		BF_PrintError("Couldn't read block");
		return -1;
	}

// 	//Then, using the index stored at the block's last bytes, go to the next block (1).
// 	next_block_p = (int*)block;
// 	next_block_p += BLOCK_SIZE - sizeof(int);
// 	memcpy(&block_number, (int*)block, sizeof(int));
// 	if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0){
// 		BF_PrintError("Couldn't read block");
// 		return -1;
// 	}
	//next_block_p points to the next block
	next_block_p = (int*)block;
	next_block_p += BLOCK_SIZE - 2*sizeof(int);

	read = (Record*)block;
	memcpy(&record, read, sizeof(Record));

	//read now points to the first (key) # of bytes of the block, where the primary key value of the Record struct is stored
	//memcpy(read, block, sizeof(Record));

	// if value is NULL, print all entries
	if(value == NULL){
		while(1){
			//if the indexed space is empty, don't print
			if (memcmp(&record, (char[sizeof(Record)]){0}, sizeof(Record)) != 0){
				//Print all the info of existing record
				printf("Found record with id: %d\n", record.id);
				printf("This record's name is: %s\n", record.name);
				printf("This record's surname is: %s\n", record.surname);
				printf("This record's address is: %s\n", record.address);
			}
			//If the value in that record isn't the one we are looking for, move Record # of bytes forward
			read += sizeof(Record);
			// If the end of a block is reached, move the block pointer to the next block, if there is one
			if ((void*)next_block_p - (void*)read < sizeof(Record)) {
				// If end of file reached, no valid key was given
				if (memcmp(next_block_p, (char[sizeof(int)]){0}, sizeof(int) ) == 0) {
					return -1;
				}
				memcpy(&block_number, next_block_p, sizeof(int));
				if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0){
					BF_PrintError("Couldn't read block");
					return -1;
				}

				read = (Record*)block;
				memcpy(&record, read, sizeof(Record));

				next_block_p = block;
				next_block_p += BLOCK_SIZE - 2*sizeof(int);
			}
			memcpy(&record, read, sizeof(Record));
		}
		return block_number;
	}

	// otherwise, print only the entry of id val, if it exists
	while (1) {

		if (memcmp(&record, (char[sizeof(Record)]){0}, sizeof(Record)) != 0
		    && memcmp(&record.id, value, key_size) == 0 ){
			//Print all the info of record with queried id
			printf("Found record with id: %d\n", record.id);
			printf("This record's name is: %s\n", record.name);
			printf("This record's surname is: %s\n", record.surname);
			printf("This record's address is: %s\n", record.address);
			return block_number;
		}

		//If the value in that record isn't the one we are looking for, move Record # of bytes forward
		read += sizeof(Record);
		// If the end of a block is reached, move the block pointer to the next block, if there is one
		if ((void*)next_block_p - (void*)read < sizeof(Record)) {
			// If end of file reached, no valid key was given
			if (memcmp(next_block_p, (char[sizeof(int)]){0}, sizeof(int) ) == 0) {
				return -1;
			}
			memcpy(&block_number, next_block_p, sizeof(int));
			if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0){
				BF_PrintError("Couldn't read block");
				return -1;
			}

			read = (Record*)block;
			memcpy(&record, read, sizeof(Record));

			next_block_p = block;
			next_block_p += BLOCK_SIZE - 2*sizeof(int);
		}
		memcpy(&record, read, sizeof(Record));

		}
}
