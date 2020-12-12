#include "Record.h"
#pragma once

/* Use to store needed information for heap file when HP_OpenFile() is used*/
typedef struct{
	int fileDesc;	// id returned by BF_OpenFile() to identify said file
	char attrType;	// type that describes key-field ('c'or 'i')
	char attrName[20];	// name of field that is key
	int attrLength;	// size of key-field
}HP_info;


/* Creates and properly initializes a heap file called "filename" */
/* Returns: 0 for success, -1 for failure */
int HP_CreateFile(	char *filename,
			char attrType,	// key type 'c' or 'i'
			char *attrName,	// name of field that is key
			int attrLength	// size of key-field
);

/* Open file named "filename" and reads the info relevant to heap file from first block */
/* In case of an error, returns NULL */
HP_info* HP_OpenFile(	char *filename);

/* Close file given by said structure */
/* Returns: 0 for success, -1 for failure*/
/* When successfully closed file -> deallocate memory used up by the structure*/
int HP_CloseFile(	HP_info *header_info);

/* Inserts record into a block of the file identified by HP_info */
/* Returns: blockId in which record was inserted upon success, -1 upon failure*/
int HP_InsertEntry(	HP_info header_info,
			Record record;	// structure in Record.h representing a record that gets stored

);

/* Deletes record with primary key 'value' */
/* Returns: 0 upon success, -1 upon failure */
int HP_DeleteEntry(	HP_info header_info,
			void *value // primary key value
);

/* Find and print all entries with primary key 'value' */
/* Returns: number of blocks read upon success, -1 upon failure*/
int HP_GetAllEntries(	HP_info header_info,
			void *value // primary key value
);
