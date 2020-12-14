#include "HP.h"
#include "Record.h"
#include "BF.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main () {
  char* filename = "sad.txt";
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

  record.id = 8;
  strcpy(record.name, "nicholas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "sex");

  HP_InsertEntry(*header_info, record);

  record.id = 9;
  strcpy(record.name, "holas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "ex");

  HP_InsertEntry(*header_info, record);

  record.id = 10;
  strcpy(record.name, "nicholas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "sex");

  HP_InsertEntry(*header_info, record);

  record.id = 11;
  strcpy(record.name, "holas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "ex");

  HP_InsertEntry(*header_info, record);

  record.id = 12;
  strcpy(record.name, "nicholas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "sex");

  HP_InsertEntry(*header_info, record);

  record.id = 13;
  strcpy(record.name, "holas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "ex");

  HP_InsertEntry(*header_info, record);

  record.id = 14;
  strcpy(record.name, "nicholas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "sex");

  HP_InsertEntry(*header_info, record);

  record.id = 15;
  strcpy(record.name, "holas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "ex");

  HP_InsertEntry(*header_info, record);


  int value = 0;
  HP_GetAllEntries(*header_info, (void*)&value);

  record.id = 1;
  strcpy(record.name, "holas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "ex");

printf("ASDASDSADD\n");
  void * blank;

BF_ReadBlock(header_info->fileDesc, 1, &blank);
memcpy(&record, blank, sizeof(Record));

printf("%d\n", record.id);
printf("%s\n", record.name);
printf("%s\n", record.surname);
printf("%sBEFORE\n\n", record.address);

blank += sizeof(Record);
memcpy(&record, blank, sizeof(Record));

printf("%d\n", record.id);
printf("%s\n", record.name);
printf("%s\n", record.surname);
printf("%s\n", record.address);

blank += sizeof(Record);
memcpy(&record, blank, sizeof(Record));

printf("%d\n", record.id);
printf("%s\n", record.name);
printf("%s\n", record.surname);
printf("%s\n", record.address);

blank += sizeof(Record);
memcpy(&record, blank, sizeof(Record));

printf("%d\n", record.id);
printf("%s\n", record.name);
printf("%s\n", record.surname);
printf("%s\n", record.address);

blank += sizeof(Record);
memcpy(&record, blank, sizeof(Record));

printf("%d\n", record.id);
printf("%s\n", record.name);
printf("%s\n", record.surname);
printf("%s\n", record.address);


BF_ReadBlock(header_info->fileDesc, 2, &blank);
blank += BLOCK_SIZE - sizeof(int);
int kapou;
memcpy(&kapou, blank, sizeof(int));
printf("%d KAPOYYYYYYYYYYYYYY\n", kapou);
blank += sizeof(int) - BLOCK_SIZE;

blank += sizeof(Record);
memcpy(&record, blank, sizeof(Record));

printf("%d\n", record.id);
printf("%s\n", record.name);
printf("%s\n", record.surname);
printf("%s\n", record.address);

blank += sizeof(Record);
memcpy(&record, blank, sizeof(Record));

printf("%d\n", record.id);
printf("%s\n", record.name);
printf("%s\n", record.surname);
printf("%s\n", record.address);

blank += sizeof(Record);
memcpy(&record, blank, sizeof(Record));

printf("%d\n", record.id);
printf("%s\n", record.name);
printf("%s\n", record.surname);
printf("%s\n", record.address);

blank += sizeof(Record);
memcpy(&record, blank, sizeof(Record));

printf("%d\n", record.id);
printf("%s\n", record.name);
printf("%s\n", record.surname);
printf("%s\n", record.address);

blank += sizeof(Record);
memcpy(&record, blank, sizeof(Record));

printf("%d\n", record.id);
printf("%s\n", record.name);
printf("%s\n", record.surname);
printf("%s\n", record.address);

BF_ReadBlock(header_info->fileDesc, 3, &blank);
blank += BLOCK_SIZE - sizeof(int);
memcpy(&kapou, blank, sizeof(int));
printf("%d KAPOYYYYYYYYYYYYYY\n", kapou);
blank += sizeof(int) - BLOCK_SIZE;

blank += sizeof(Record);
memcpy(&record, blank, sizeof(Record));

printf("%d\n", record.id);
printf("%s\n", record.name);
printf("%s\n", record.surname);
printf("%s\n", record.address);

blank += sizeof(Record);
memcpy(&record, blank, sizeof(Record));

printf("%d\n", record.id);
printf("%s\n", record.name);
printf("%s\n", record.surname);
printf("%s\n", record.address);

blank += sizeof(Record);
memcpy(&record, blank, sizeof(Record));

printf("%d\n", record.id);
printf("%s\n", record.name);
printf("%s\n", record.surname);
printf("%s\n", record.address);

blank += sizeof(Record);
memcpy(&record, blank, sizeof(Record));

printf("%d\n", record.id);
printf("%s\n", record.name);
printf("%s\n", record.surname);
printf("%s\n", record.address);

blank += sizeof(Record);
memcpy(&record, blank, sizeof(Record));

printf("%d\n", record.id);
printf("%s\n", record.name);
printf("%s\n", record.surname);
printf("%s\n", record.address);


  value = 1;
//  HP_GetAllEntries(*header_info, (void*)&value);

printf("asdaasdadasdasd\n");

int* kati = 0;
  HP_DeleteEntry(*header_info, kati);

  //FOR TOMOROOOOOOW

  
  //HP_GetAllEntries(*header_info, NULL);
}
