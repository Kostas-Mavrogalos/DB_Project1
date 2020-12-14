#include "HP.h"
#include "Record.h"
#include "BF.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main () {
  char* filename = "sad.txt";
  BF_Init();
  HP_CreateFile(filename, 'c', "DDA", 5);

  HP_info* header_info = HP_OpenFile(filename);
  Record record;

  record.id = 0;
  strcpy(record.name, "nicholas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "sex");

  HP_InsertEntry(&header_info, record);

  record.id = 1;
  strcpy(record.name, "holas");
  strcpy(record.surname, "cage");
  strcpy(record.address, "ex");

  HP_InsertEntry(*header_info, record);

  HP_InsertEntry(*header_info, record);
  HP_InsertEntry(*header_info, record);
  HP_InsertEntry(*header_info, record);
  HP_InsertEntry(*header_info, record);
  HP_InsertEntry(*header_info, record);
  HP_InsertEntry(*header_info, record);
  HP_InsertEntry(*header_info, record);
  HP_InsertEntry(*header_info, record);
  HP_InsertEntry(*header_info, record);
  HP_InsertEntry(*header_info, record);
  HP_InsertEntry(*header_info, record);

  HP_GetAllEntries(*header_info, NULL);

  HP_GetAllEntries(*header_info, 0);

}
