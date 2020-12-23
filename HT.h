#include "Record.h"
#pragma once

typedef struct {
  int fileDesc;
  char attrType;
  char attrName[20];
  int attrLength;
  long int numBuckets;
}HT_info;

int hashFunction(long int numBuckets, void* id);

int HT_CreateIndex(char* filename, char attrType, char* attrName, int attrLength, long int numBuckets);

HT_info* HT_OpenIndex(char* filename);

int HT_CloseIndex(HT_info *header_info);

int HT_InsertEntry(HT_info header_info, Record record);

int HT_DeleteEntry(HT_info header_info, void* value);

int HT_GetAllEntries(HT_info header_info, void* value);

int HashStatistics(char* filename);
