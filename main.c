#include "HP.h"
#include "Record.h"
#include "BF.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HT.h"

int main () {
  char* filename = "sad.txt";
	void *block;

  HT_CreateIndex(filename, 'i', "DDA", 4, 500);

  HT_info* header_info = HT_OpenIndex(filename);

  int index;
  BF_ReadBlock(header_info->fileDesc, 1, &block);
  void* next;
  int block_number;
  next = block;
  next += BLOCK_SIZE - sizeof(int);
  memcpy(&block_number, next, sizeof(int));
  printf("%d BLOCK #\n", block_number);
  BF_ReadBlock(header_info->fileDesc, block_number, &block);
  for (long int i=0; i<10; i++) {
    memcpy(&index, (int*)block, sizeof(int));
    printf("%d INDEX\n", index);
    block+= sizeof(int);
  }
  Record record;

  record.id = 0;
  strcpy(record.name, "nicholas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "sex");

  HT_InsertEntry(*header_info, record);

  record.id = 1;
  strcpy(record.name, "holas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "ex");

  HT_InsertEntry(*header_info, record);

  record.id = 2;
  strcpy(record.name, "nicholas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "sex");

  HT_InsertEntry(*header_info, record);

  record.id = 3;
  strcpy(record.name, "holas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "ex");

  HT_InsertEntry(*header_info, record);

  record.id = 4;
  strcpy(record.name, "nicholas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "sex");

  HT_InsertEntry(*header_info, record);

  record.id = 5;
  strcpy(record.name, "holas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "ex");

  HT_InsertEntry(*header_info, record);

  record.id = 6;
  strcpy(record.name, "nicholas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "sex");

  HT_InsertEntry(*header_info, record);

  record.id = 7;
  strcpy(record.name, "holas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "ex");

  HT_InsertEntry(*header_info, record);

  int val = 2;
  Record rec;
  Record r;
  BF_ReadBlock(header_info->fileDesc, 1, &block);
  memcpy(&rec, block, sizeof(Record));

  HT_DeleteEntry(*header_info, &val);
  BF_ReadBlock(header_info->fileDesc, 1, &block);
  memcpy(&rec, block, sizeof(Record));

  if(memcmp(&rec, (char[sizeof(Record)]){0}, sizeof(Record))==0){
	printf("That's a wet ass pussy.\n");
  }
  block += 1*sizeof(Record);
  memcpy(&rec, block, sizeof(Record));

  BF_ReadBlock(header_info->fileDesc, 1, &block);
  block += 2*sizeof(Record);
  memcpy(&rec, (Record*)block, sizeof(Record));
  printf("Id: %d\n", rec.id);
  printf("Name: %s\n", rec.name);
  val = 1;
  int* dick = NULL;
  printf("ALL MOTHEFUCKAAAA\n");
  HT_GetAllEntries(*header_info, dick);
}
