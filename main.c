#include "HP.h"
#include "Record.h"
#include "BF.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main () {
  char* filename = "sad.txt";
	void *block;
  BF_Init();
  HP_CreateFile(filename, 'i', "DDA", 4);

  HP_info* header_info = HP_OpenFile(filename);

  Record record;

  record.id = 0;
  strcpy(record.name, "nicholas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "sex");

  HP_InsertEntry(*header_info, record);

  record.id = 1;
  strcpy(record.name, "holas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "ex");

  HP_InsertEntry(*header_info, record);

  record.id = 2;
  strcpy(record.name, "nicholas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "sex");

  HP_InsertEntry(*header_info, record);

  record.id = 3;
  strcpy(record.name, "holas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "ex");

  HP_InsertEntry(*header_info, record);

  record.id = 4;
  strcpy(record.name, "nicholas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "sex");

  HP_InsertEntry(*header_info, record);

  record.id = 5;
  strcpy(record.name, "holas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "ex");

  HP_InsertEntry(*header_info, record);

  record.id = 6;
  strcpy(record.name, "nicholas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "sex");

  HP_InsertEntry(*header_info, record);

  record.id = 7;
  strcpy(record.name, "holas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "ex");

  HP_InsertEntry(*header_info, record);

  int val = 2;
  Record rec;
  Record r;
  BF_ReadBlock(header_info->fileDesc, 1, &block);
  memcpy(&rec, block, sizeof(Record));

  HP_DeleteEntry(*header_info, &val);
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
  HP_GetAllEntries(*header_info, dick);
}
