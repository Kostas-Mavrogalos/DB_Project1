#include "HT.h"
#include "Record.h"
#include "BF.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int hashFunction(long int numBuckets, void* id){
	// uses id to find bucket id. Return a hash_key where 0 <= hash_key < numBuckets
	int bucket_id;
	int hash;
	memcpy(&hash, id, sizeof(int));
	bucket_id = hash % numBuckets;
	return bucket_id;
}

int HT_CreateIndex(char* filename, char attrType, char* attrName, int attrLength, long int numBuckets)
{
	BF_Init();
	int fileDesc;
	int bucket_index;
	int next_block;
	int curr_block;
	int index = 0;
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

	//Line intentionally left blank
	memcpy(block, &attrLength, sizeof(int));
	block += sizeof(int);
	memcpy(block, &numBuckets, sizeof(long int));

	// Use block #1 and beyond to store the indexes of the buckets.
 	if (BF_AllocateBlock(fileDesc) < 0 ) {
		BF_PrintError("Couldn't allocate block");
		return -1;
	}

	next_block = 1;
	memcpy(end_of_block, &next_block, sizeof(int));

	if (BF_WriteBlock(fileDesc, 0) < 0){
		BF_PrintError("Couldnt' write block");
		return -1;
	}

 	if (BF_ReadBlock(fileDesc, 1, &block) < 0 ) {
		BF_PrintError("Couldn't read block");
		return -1;
	}


	curr_block = 1;																																// curr_block holds the pointer of the block where bucket indexes are stored.
	hash_block = block;
	end_of_block = block;
	end_of_block += BLOCK_SIZE - sizeof(int);
 	// From block #1 up to block #numBuckets, create the first block of each bucket
  	for (int i=0; i < numBuckets; i++) {

		// if curr_block has no more space for indices, allocate a new block and use the last 4 bytes of curr_block to store the pointer
			if ( end_of_block - hash_block <= sizeof(int)) {

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

				if (BF_WriteBlock(fileDesc, curr_block) < 0){
  				BF_PrintError("Couldnt' write block");
  				return -1;
  			}

				curr_block = bucket_index;

				end_of_block = block;
				end_of_block += BLOCK_SIZE - sizeof(int);

				index = 0;
			}

			//Create bucket and store index of its corresponding block
    	if (BF_AllocateBlock(fileDesc) < 0 ) {
  			BF_PrintError("Couldn't allocate block");
  			return -1;
  		}

			if (BF_ReadBlock(fileDesc, curr_block, &hash_block) < 0 ) {
				BF_PrintError("Couldn't read block");
				return -1;
			}
			end_of_block = hash_block;
			end_of_block +=BLOCK_SIZE-sizeof(int);
			hash_block+=sizeof(int)*index;

			bucket_index = BF_GetBlockCounter(fileDesc)-1;
			memcpy((int*)hash_block, &bucket_index ,sizeof(int));
			index++;
			hash_block +=sizeof(int);

			if (BF_WriteBlock(fileDesc, curr_block) < 0){
				BF_PrintError("Couldnt' write block");
				return -1;
			}

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
	bucket = hashFunction(header_info.numBuckets, &(record.id));

	// Read the block where the bucket starts
	if (BF_ReadBlock(header_info.fileDesc, 0, &block) < 0 ) {
		BF_PrintError("Couldn't read file");
		return -1;
	}

	next_block_p = block;
	next_block_p += BLOCK_SIZE - sizeof(int);
	// memcpy(&block_number, (int*)next_block_p, sizeof(int));
	memcpy(&block_number, next_block_p, sizeof(int));

	// Read block #1 where the bucket indexes start
	if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0 ) {
		BF_PrintError("Couldn't read file");
		return -1;
	}

	bucket_index = bucket;

	//If the index the Hash Function returned so big that the number wouldn't fit in the first index block, reduce it by 127
	//Inside an index block of 512 bytes, 4 are the pointer to the next block. The other 508 are where the indexes are stored and
	//since sizeof(int) is 4 bytes, 508/4=127 integers can fit.
	while(bucket_index >= (BLOCK_SIZE - sizeof(int))/4)
	{
		bucket_index -= (BLOCK_SIZE - sizeof(int))/4;
		// memcpy(&block_number, (int*)next_block_p, sizeof(int));
		memcpy(&block_number, next_block_p, sizeof(int));

		if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0){
			BF_PrintError("Couldn't read block");
			return -1;
		}

		next_block_p = block;
		next_block_p += BLOCK_SIZE - sizeof(int);
	}

	next_block_p = block;
	next_block_p += bucket_index*sizeof(int);
	// memcpy(&block_number, (int*)next_block_p, sizeof(int));
	memcpy(&block_number, next_block_p, sizeof(int));

	// read the first block of the bucket where the new entry belongs
	if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0){
		BF_PrintError("Couldn't read block");
		return -1;
	}

	next_block_p = block;
	next_block_p += BLOCK_SIZE - 2*sizeof(int);

	num_records_p = block;
	num_records_p += BLOCK_SIZE - sizeof(int);
	memcpy(&num_of_records, (int*)num_records_p, sizeof(int));

	// the following condition checks whether the block is already full
	while (num_of_records ==  (BLOCK_SIZE - 2*sizeof(int))/sizeof(Record))
	{
		if (memcmp(next_block_p, (char[sizeof(int)]){0}, sizeof(int) ) == 0) {

			if (BF_AllocateBlock(header_info.fileDesc) < 0 ) {
				BF_PrintError("Couldn't allocate block");
				return -1;
			}
			int prev_block = block_number;
			block_number = BF_GetBlockCounter(header_info.fileDesc)-1;
			if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0){
				BF_PrintError("Couldn't read block");
				return -1;
			}

			memcpy((int*)next_block_p, &block_number, sizeof(int));

			if (BF_WriteBlock(header_info.fileDesc, prev_block) < 0){
				BF_PrintError("Couldn't write block");
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
		num_records_p = block;
		num_records_p += BLOCK_SIZE - sizeof(int);
		memcpy(&num_of_records, (int*)num_records_p, sizeof(int));
	}

	first_available = (Record*)block;
	// Go to the first available spot for insertion
	while (memcmp(first_available, (char[sizeof(Record)]){0}, sizeof(Record) ) != 0){

		first_available += sizeof(Record);

	}

	//Store the record inside the block
	memcpy((Record*)first_available, &record, sizeof(Record));

	//"Pull" the # of records inside the block and incerent it
	memcpy(&num_of_records, num_records_p, sizeof(int));
	num_of_records++;
	memcpy(num_records_p, &num_of_records, sizeof(int));

	if (BF_WriteBlock(header_info.fileDesc, block_number) < 0 ) {
		BF_PrintError("Couldn't write block");
		return -1;
	}

	return bucket;

}

int HT_DeleteEntry(HT_info header_info, void* value) {
	int block_number;
	int bucket;
	int bucket_index;
	void* block;
	void* next_block_p;
	int key_size = header_info.attrLength;
	void* record;																																	//a Record that will be empty, to delete the record we need to
	Record read;

	bucket = hashFunction(header_info.numBuckets, value);

	// Read the header block
	if (BF_ReadBlock(header_info.fileDesc, 0, &block) < 0 ) {
		BF_PrintError("Couldn't read file");
		return -1;
	}

	next_block_p = block;
	next_block_p += BLOCK_SIZE - sizeof(int);
	memcpy(&block_number, (int*)next_block_p, sizeof(int));

	// Read block #1 where the bucket indexes start
	if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0 ) {
		BF_PrintError("Couldn't read file");
		return -1;
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
	memcpy(&block_number, (int*)next_block_p, sizeof(int));

	if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0){
		BF_PrintError("Couldn't read block");
		return -1;
	}

	//next_block_p points to the next block
	next_block_p = block;
	next_block_p += BLOCK_SIZE - 2*sizeof(int);

	//record now points to the first sizeof(Record) # of bytes of the block, where record will be searched
	record = block;
	memcpy(&read, (Record*)record, sizeof(Record));

	while (1) {
		if (memcmp(&(read.id), (int*)value, key_size) == 0){
			//Empty the value and fill it with 0's
			memset(record, 0, sizeof(Record));
			memcpy(&read, (Record*)record, sizeof(Record));

			if (BF_WriteBlock(header_info.fileDesc, block_number) < 0 ) {
				BF_PrintError("Couldn't write block");
				return -1;
			}
			return 0;
		}
		if (next_block_p - (void*)record < sizeof(Record)) {												//For when the available space in the block isn't enough for a Record to fit
			if (memcmp(next_block_p, (char[sizeof(int)]){0}, sizeof(int))==0) {				//If there isn't a next block
				return -1;
			}
			memcpy(&block_number, next_block_p, sizeof(int));
			if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0){
				BF_PrintError("Couldn't read block");
				return -1;
			}
			record = block;
			memcpy(&read, (Record*)record, sizeof(Record));
			//Line intentionally left blank

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

	int num_of_blocks = 2;
	int bucket;
	int block_number;
	int bucket_index;
	int curr_block;
	int key_size = header_info.attrLength;
	int records;
	void* block;
	void* hash_block;
	void* next_block_p;
	void* end_of_block;
	void *num_records_p;
	void* read;
	Record record;

	// Read the header block
	if (BF_ReadBlock(header_info.fileDesc, 0, &block) < 0 ) {
		BF_PrintError("Couldn't read file");
		return -1;
	}

	next_block_p = block;
	next_block_p += BLOCK_SIZE - sizeof(int);
	memcpy(&block_number, (int*)next_block_p, sizeof(int));
	// Read block #1 where the bucket indexes start
	if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0 ) {
		BF_PrintError("Couldn't read file");
		return -1;
	}

	curr_block = block_number;

	end_of_block = block;
	end_of_block += BLOCK_SIZE - sizeof(int);
	//Print all values
	if(value == NULL) {
		int index = 0;
		hash_block = block;
		for (int i=0;  i<header_info.numBuckets; i++) {
			memcpy(&block_number, (int*)hash_block, sizeof(int));

			if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0 ) {
				BF_PrintError("Couldn't read file for bucket index 1");
				return -1;
			}
			next_block_p = block;
			next_block_p += BLOCK_SIZE - 2*sizeof(int);
			num_records_p = block;
			num_records_p += BLOCK_SIZE - sizeof(int);
			memcpy(&records, num_records_p, sizeof(int));
			num_of_blocks++;
			read = (Record*)block;
			memcpy(&record, block, sizeof(Record));

			while (1) {
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
					printf("\n");
					// If end of bucket reached, go to the next one
					if (memcmp(next_block_p, (char[sizeof(int)]){0}, sizeof(int) ) == 0) {
						break;
					}
					memcpy(&block_number, next_block_p, sizeof(int));
					if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0) {
						BF_PrintError("Couldn't read block (trying to visit next block)");
						return -1;
					}
					num_of_blocks++;
					read = (Record*)block;
					memcpy(&record, read, sizeof(Record));

					next_block_p = block;
					next_block_p += BLOCK_SIZE - 2*sizeof(int);
				}
				memcpy(&record, read, sizeof(Record));
			}

			index++;

			if (BF_ReadBlock(header_info.fileDesc, curr_block, &block) < 0) {
				BF_PrintError("Couldn't read block for bucket index 2");
				return -1;
			}

			hash_block = block;
			hash_block += index*sizeof(int);
			end_of_block = block;
			end_of_block += BLOCK_SIZE - sizeof(int);

			if (end_of_block - hash_block <= sizeof(int)) {
				memcpy(&block_number, end_of_block, sizeof(int));

				if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0) {
					BF_PrintError("Couldn't read block for bucket index 2");
					return -1;
				}

				curr_block = block_number;
				hash_block = block;
				end_of_block = block;
				end_of_block += BLOCK_SIZE - sizeof(int);
				index = 0;
			}
		}
		return num_of_blocks;
	}

	bucket = hashFunction(header_info.numBuckets, value);
	bucket_index = bucket;

	while(bucket_index >= (BLOCK_SIZE - sizeof(int))/4)
	{
		bucket_index -= (BLOCK_SIZE - sizeof(int))/4;
		memcpy(&block_number, (int*)next_block_p, sizeof(int));

		if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0){
			BF_PrintError("Couldn't read block");
			return -1;
		}
		num_of_blocks++;
		next_block_p = block;
		next_block_p += BLOCK_SIZE - sizeof(int);
	}
	next_block_p = block;
	next_block_p += bucket_index*sizeof(int);

	memcpy(&block_number, (int*)next_block_p, sizeof(int));

	if (BF_ReadBlock(header_info.fileDesc, block_number, &block) < 0){
		BF_PrintError("Couldn't read block");
		return -1;
	}
	num_of_blocks++;

	//next_block_p points to the next block
	next_block_p = block;
	next_block_p += BLOCK_SIZE - 2*sizeof(int);
	read = block;
	memcpy(&record, read, sizeof(Record));

	//Print one value, the one with id == value
	while (1) {
		if (memcmp(&record, (char[sizeof(Record)]){0}, sizeof(Record)) != 0 && memcmp(&(record.id), value, key_size) == 0) {
			//Print all the info of record with queried id
			printf("Found record with id: %d\n", record.id);
			printf("This record's name is: %s\n", record.name);
			printf("This record's surname is: %s\n", record.surname);
			printf("This record's address is: %s\n", record.address);
			return num_of_blocks;
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
			num_of_blocks++;

			read = (Record*)block;
			memcpy(&record, read, sizeof(Record));

			next_block_p = block;
			next_block_p += BLOCK_SIZE - 2*sizeof(int);
		}
		memcpy(&record, read, sizeof(Record));
	}
}

int HashStatistics(char* filename) {

	int min = INT_MAX, max = 0;
	long int local_sum, total_sum = 0, block_count = 0;
	int block_number;
	int record_count;
	int index_blocks = 1;						//To do BF_GetBlockCounter - index_blocks - header block to get the # of blocks with records
	int index = 0;
	int curr_block;
	HT_info* header_info;
	void* block;
	void* num_records_p;
	void* next_block_p;
	void* hash_block;
	void* end_of_block;

	header_info = HT_OpenIndex(filename);

	// Read the block where the bucket starts
	if (BF_ReadBlock(header_info->fileDesc, 0, &block) < 0 ) {
		BF_PrintError("Couldn't read file");
		return -1;
	}

	next_block_p = block;
	next_block_p += BLOCK_SIZE - sizeof(int);
	hash_block = block;
	memcpy(&block_number, (int*)next_block_p, sizeof(int));
	// Read block #1 where the bucket indexes start
	if (BF_ReadBlock(header_info->fileDesc, block_number, &block) < 0 ) {
		BF_PrintError("Couldn't read file");
		return -1;
	}

	curr_block = block_number;

	printf("This file has %d blocks inside\n", BF_GetBlockCounter(header_info->fileDesc));
	int overflow_blocks;

	//For each bucket
	for (int i = 0; i<header_info->numBuckets; i++) {
		overflow_blocks = 0;
		local_sum = 0;
		memcpy(&block_number, (int*)hash_block, sizeof(int));

		if (BF_ReadBlock(header_info->fileDesc, block_number, &block) < 0 ) {
			BF_PrintError("Couldn't read file for bucket index 1");
			return -1;
		}

		//"Pull" the needed variables and add the local_sum, where the local # of records is stored
		num_records_p = block;
		num_records_p += BLOCK_SIZE - sizeof(int);
		memcpy(&record_count, num_records_p, sizeof(int));
		local_sum += record_count;
		memcpy(&record_count, block, sizeof(int));

		while (memcmp(next_block_p, (char[sizeof(int)]){0}, sizeof(int) ) != 0) {
			num_records_p = block;
			num_records_p += BLOCK_SIZE - sizeof(int);
			memcpy(&record_count, num_records_p, sizeof(int));
			local_sum += record_count;
			memcpy(&block_number, next_block_p, sizeof(int));

			if (BF_ReadBlock(header_info->fileDesc, block_number, &block) < 0 ) {
				BF_PrintError("Couldn't read file for bucket index 1");
				return -1;
			}

			next_block_p = block;
			next_block_p += BLOCK_SIZE - 2*sizeof(int);

			//Line intentionally left blank
			overflow_blocks++;
		}

		if (overflow_blocks > 1) {
			printf("Found bucket number %d with overflow blocks, totalling: %d blocks\n", i, overflow_blocks);
		}

		if (local_sum < min) min = local_sum;
		if (local_sum > max) max = local_sum;
		total_sum += local_sum;
		index++;

		if (BF_ReadBlock(header_info->fileDesc, curr_block, &block) < 0) {
			BF_PrintError("Couldn't read block for bucket index 2");
			return -1;
		}

		hash_block = block;
		hash_block += index*sizeof(int);
		end_of_block = block;
		end_of_block += BLOCK_SIZE - sizeof(int);

		if (end_of_block - hash_block < sizeof(int)) {
			memcpy(&block_number, end_of_block, sizeof(int));
			if (BF_ReadBlock(header_info->fileDesc, block_number, &block) < 0) {
				BF_PrintError("Couldn't read block for bucket index 2");
				return -1;
			}
			curr_block = block_number;
			hash_block = block;
			end_of_block = block;
			end_of_block += BLOCK_SIZE - sizeof(int);
			index = 0;
			index_blocks++;
		}
	}

	printf("The maximum number of records in the file were: %d\n", max);
	printf("The minimum number of records in the file were: %d\n", min);
	printf("The mean number of records in the file were: %lf\n", (double)total_sum/header_info->numBuckets);

	printf("The mean number of blocks with records is: %d\n", BF_GetBlockCounter(header_info->fileDesc) - index_blocks - 1); //The -1 is the header block
}
