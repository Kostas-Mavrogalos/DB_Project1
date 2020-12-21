#include "HT.h"
#include "Record.h"
#include "BF.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hashFunction(int numBuckets, void* id){
	// uses id to find bucket id. Return a hash_key where 0 <= hash_key < numBuckets
}

int HT_CreateIndex(char* filename, char attrType, char* attrName, int attrLength, long int numBuckets)
{
	BF_Init();
	int fileDesc;
	int bucket_index;
	int next_block;
	void *block;
	void *hash_block;
	void *end_of_block;
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
	
	end_of_block = block;
	end_of_block += BLOCK_SIZE - sizeof(int);
	
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
	block += sizeof(long int);
	
	// Use block #1 and beyond to store the indices of the buckets.
 	if (BF_AllocateBlock(fileDesc) < 0 ) {
		BF_PrintError("Couldn't allocate block");
		return -1;
	}

 	if (BF_ReadBlock(fileDesc, 1, &block) < 0 ) {
		BF_PrintError("Couldn't read block");
		return -1;
	}
	
	if (BF_WriteBlock(fileDesc, 0) < 0){
		BF_PrintError("Couldnt' write block");
		return -1;
	}
	
	next_block = 1;
	memcpy(end_of_block, &next_block, sizeof(int)); 
	
	hash_block = block;
	end_of_block = block;
	end_of_block += BLOCK_SIZE - sizeof(int);
 	// From block #1 up to block #numBuckets, create the first block of each bucket
  	for (int i=0; i < numBuckets; i++) {
		
    		if (BF_AllocateBlock(fileDesc) < 0 ) {
  			BF_PrintError("Couldn't allocate block");
  			return -1;
  		}
		
		bucket_index = BF_GetBlockCounter(fileDesc)-1;
   		if (BF_ReadBlock(fileDesc, bucket_index, &block) < 0 ) {
  			BF_PrintError("Couldn't read block");
  			return -1;
  		}
		
		if ( (int*)end_of_block - (int*)hash_block < sizeof(int)){
			if (BF_AllocateBlock(fileDesc) < 0 ) {
  				BF_PrintError("Couldn't allocate block");
  				return -1;
  			}
			bucket_index = BF_GetBlockCounter(fileDesc) - 1;
   			if (BF_ReadBlock(fileDesc, bucket_index, &block) < 0 ) {
  				BF_PrintError("Couldn't read block");
  				return -1;
  			}
			hash_block = block;
			memcpy((int*)end_of_block, &bucket_index, sizeof(int));
			end_of_block = block;
			end_of_block += BLOCK_SIZE - sizeof(int);
		}
		memcpy((int*)hash_block, &bucket_index ,sizeof(int));
		hash_block += sizeof(int); 

  		if (BF_WriteBlock(fileDesc, bucket_index) < 0){
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

int HT_CloseIndex(HT_info *header_info) {
	if (BF_CloseFile(header_info->fileDesc) < 0) {
		BF_PrintError("Couldn't close file");
		return -1;
	}

	free(header_info);

	return 0;
}

int HT_InsertEntry(HT_info header_info, Record record) {
	int bucket;
	int bucket_index;
	int block_number;
	int num_of_records;
	void *block;
	void *num_records_p;
	void *next_block_p;
	void* first_available;
	
	// Use the provided hashFunction to find which bucket corresponds to the given record's id
	bucket = hashFunction(header_info.numBuckets, &Record.id);
	
	// Read the block where the bucket starts
	if (BF_ReadBlock(fileDesc, 0, &block) < 0 ) {
		BF_PrintError("Couldn't read file");
		return NULL;
	}
	
	next_block_p = block;
	next_block_p += BLOCK_SIZE - sizeof(int);
	memcpy(&block_number, (int*)next_block_p, sizeof(int));
	
	// Read block #1 where the bucket indexes start
	if (BF_ReadBlock(fileDesc, block_number, &block) < 0 ) {
		BF_PrintError("Couldn't read file");
		return NULL;
	}
	
	bucket_index = bucket;
	while(bucket_index >= (BLOCK_SIZE - sizeof(int))/4)
	{
		bucket_index -= (BLOCK_SIZE - sizeof(int))/4;
		memcpy(&block_number, (int*)next_block_p, sizeof(int));
		
		if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0){
			BF_PrintError("Couldn't read block");
			return -1;
		}
		next_block_p = block;
		next_block_p += BLOCK_SIZE - sizeof(int);
	}
	
	next_block_p = block;
	next_block_p += bucket_index*sizeof(int);
	
	if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0){
		BF_PrintError("Couldn't read block");
		return -1;
	}
	
	next_block_p = block;
	next_block_p += BLOCK_SIZE - 2*sizeof(int);
	
	num_records_p = block;
	num_records_p += BLOCK_SIZE - sizeof(int);
	memcpy(&num_of_records, (int*)num_records_p, sizeof(int));
	
	while (num_of_records ==  (BLOCK_SIZE - 2*sizeof(int))/sizeof(Record))
	{
		if (memcmp(next_block_p, (char[sizeof(int)]){0}, sizeof(int) ) == 0){
			if (BF_AllocateBlock(header_info.fileDesc) < 0 ) {
				BF_PrintError("Couldn't allocate block");
				return -1;
			}
			block_number = BF_GetBlockCounter(header_info.fileDesc)-1
			if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0){
				BF_PrintError("Couldn't read block");
				return -1;
			}
			
			memcpy((int*)next_block_p, &block_number, sizeof(int));
			
			if (BF_WriteBlock(header_info.fileDesc, block_number) < 0){
				BF_PrintError("Couldn't read block");
				return -1;
			}
		}
		memcpy(&block_number, next_block_p, sizeof(int));
		if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0){
			BF_PrintError("Couldn't read block");
			return -1;
		}
		next_block_p = block;
		next_block_p += BLOCK_SIZE - 2*sizeof(int);
	}
	
	first_available = (Record*)block;
	// Go to the last block of this bucket for insertion
	while (memcmp(first_available, (char[sizeof(Record)]){0}, sizeof(Record) ) != 0){
		
		first_available += sizeof(Record);
		
	}
	
	memcpy((Record*)first_available, &record, sizeof(Record));
	
	if (BF_WriteBlock(header_info.fileDesc, block_number) < 0 ) {
		BF_PrintError("Couldn't write block");
		return -1;
	}
	
	return bucket;
	
}

int HT_DeleteEntry(HT_info header_info, void* value) {
	int block_number;
	void* block;
	void* next_block_p;
	int key_size = header_info.attrLength;
	void* record;							//To clear the Record that is to be deleted
	Record read;

	block_number = BF_GetBlockCounter(header_info.fileDesc);

	if (block_number == 1) return -1;

	// Go to the bucket corresponding to the hash_key of given value
	bucket = hashFunction(header_info.numBuckets, value) + 1;
	
	if (BF_ReadBlock(header_info.fileDesc, bucket, &block) < 0){				//Read the block that has data
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
		if (next_block_p - (void*)record < sizeof(Record)) {				//For when the available space in the block isn't enough for a Record to fit
			if (memcmp(next_block_p, (char[sizeof(int)]){0}, sizeof(int))==0) {	//If there isn't a next block
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

int HT_GetAllEntries(HT_info header_info, void* value) {

}
