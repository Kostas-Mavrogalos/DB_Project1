#include "HT.h"
#include "Record.h"
#include "BF.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int HT_CreateIndex(char* filename, char attrType, char* attrName, int attrLength, long int numBuckets)
{
	BF_Init();
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
	
	// Use block #0 as header of the hash file.
 	if (BF_AllocateBlock(fileDesc) < 0 ) {
		BF_PrintError("Couldn't allocate block");
		return -1;
	}

 	if (BF_ReadBlock(fileDesc, 0, &block) < 0 ) {
		BF_PrintError("Couldn't read block");
		return -1;
	}
	
	// Store all necessary data for HT_info which is initialized later
	memcpy(block, &fileDesc, sizeof(int));
	block += sizeof(int);

	memcpy(block, &attrType, sizeof(char));
	block += sizeof(char);

	strcpy(block, attrName);
	block += strlen(attrName) + 1;

	memcpy(block, &attrLength, sizeof(int));
	block += sizeof(int);
	
	memcpy(block, &numBuckets, sizeof(long int));

	if (BF_WriteBlock(fileDesc, 0) < 0){
		BF_PrintError("Couldnt' write block");
		return -1;
	}

 	// From block #1 up to block #numBuckets, create the first block of each bucket
  	for (int i=1; i <= numBuckets; i++) {
    		if (BF_AllocateBlock(i) < 0 ) {
  			BF_PrintError("Couldn't allocate block");
  			return -1;
  		}

   		if (BF_ReadBlock(i, 0, &block) < 0 ) {
  			BF_PrintError("Couldn't read block");
  			return -1;
  		}

  		if (BF_WriteBlock(i, 0) < 0){
  			BF_PrintError("Couldnt' write block");
  			return -1;
  		}

  	}

	if (BF_CloseFile(fileDesc)< 0 ) {
		BF_PrintError("Couldn't close file");
		return -1;
	}

	return 0;
}

HT_info* HT_OpenIndex(char* filename) {
	int fileDesc;
	void* block;
	
	if ( (fileDesc = BF_OpenFile(filename)) < 0 ) {
		BF_PrintError("Couldn't open file");
	  return NULL;
	}
	
	// Read the first block where we stored the vital data
	if (BF_ReadBlock(fileDesc, 0, &block) < 0 ) {
		BF_PrintError("Couldn't read file");
		return NULL;
	}

	// Read all data from header block
	HT_info* header_info = malloc(sizeof(HT_info));

	memcpy(&(header_info->fileDesc), block, sizeof(int));
	block+=sizeof(int);

	memcpy(&(header_info->attrType), block, sizeof(char));
	block+=sizeof(char);

	strcpy(header_info->attrName, block);
	block+=strlen(block) + 1;

	memcpy(&(header_info->attrLength), block, sizeof(int));
	block+=sizeof(int);
	
	memcpy(&(header_info->numBuckets), block, sizeof(long int));
	return header_info;
}

int HT_CloseIndex(char* filename) {
	if (BF_CloseFile(header_info->fileDesc) < 0) {
		BF_PrintError("Couldn't close file");
		return -1;
	}

	free(header_info);

	return 0;
}

int HT_InsertEntry(HT_info header_info, Record record) {
	int bucket;
	int block_number;
	int num_of_records;
	void *block;
	void *num_records_p;
	void *next_block_p;
	void* first_available;
	
	// Use the provided hashFunction to find which bucket corresponds to the given record's id
	bucket = hashFunction(header_info.numBuckets, Record.id) + 1;
	
	// Read the block where the bucket starts
	if (BF_ReadBlock(fileDesc, bucket, &block) < 0 ) {
		BF_PrintError("Couldn't read file");
		return NULL;
	}
	
	next_block_p = block;
	next_block_p += BLOCK_SIZE - 2*sizeof(int);
	
	// Go to the last block of this bucket for insertion
	while (memcmp(next_block_p, (char[sizeof(int)]){0}, sizeof(int) ) != 0){
		
		memcpy(&block_number, next_block_p, sizeof(int));
		if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0){
			BF_PrintError("Couldn't read block");
			return -1;
		}
		next_block_p = block;
		next_block_p += BLOCK_SIZE - 2*sizeof(int);
		
	}

	// Go to the last 4 bytes and retrieve the number of records
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
	block_number = BF_GetBlockCounter(header_info.fileDesc);

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

	printf("%d records in block# %d for id %d\n",num_of_records, block_number, record.id);
	if (BF_WriteBlock(header_info.fileDesc, block_number) < 0 ) {
		BF_PrintError("Couldn't write block");
		return -1;
	}

	return block_number;
}

int HT_DeleteEntry(HT_info header_info, void* value) {

}

int HT_GetAllEntries(HT_info header_info, void* value) {

}
