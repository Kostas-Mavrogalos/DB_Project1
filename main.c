#include "HP.h"
#include "Record.h"
#include "BF.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main () {
  char* filename = "sad.txt";
  BF_Init();
  HP_CreateFile(filename, 'i', "DDA", 5);

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

  int value = 0;
  HP_GetAllEntries(*header_info, &value);

  HP_GetAllEntries(*header_info, NULL);
}
