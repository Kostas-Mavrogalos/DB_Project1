#include "HT.h"
#include "Record.h"
#include "BF.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int HT_CreateIndex(char* filename, char attrType, char* attrName, int attrLength, long int numBuckets)
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

}

int HT_CloseIndex(char* filename) {

}

int HT_InsertEntry(HT_info header_info, Record record) {

}

int HT_DeleteEntry(HT_info header_info, void* value) {

}

int HT_GetAllEntries(HT_info header_info, void* value) {

}
