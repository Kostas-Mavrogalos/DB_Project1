#include "HP.h"
#include "Record.h"
#include "BF.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HT.h"

int read_records(char* filename) {
  FILE* ptr;
  ptr = fopen(filename, "r");
  char buffer[200];

  char exclude[3] = "\",";                                                      //There are the characters that will be excluded when reading the .txt file

  Record record;
  fscanf(ptr, "{%s}", buffer);                                                  //Read the line from the file

  char* split;                                                                  //The string in which the id will be stored briefly before insertion

  HP_CreateFile("hp_file", 'i', "DDA", 4);
  HP_info* header_info = HP_OpenFile("hp_file");

  while (fscanf(ptr, "\n{%s}", buffer) != EOF) {
    split = strtok(buffer, ",\"");                                              //Split the string to get the id
    record.id = atoi(split);
    strcpy(record.name, strtok(NULL, exclude));                                 //Split the string to get the name and store
    strcpy(record.surname, strtok(NULL, exclude));                              //Split the string to get the surname and store
    strcpy(record.address, strtok(NULL, exclude));                              //Split the string to get the address and store
    HP_InsertEntry(*header_info, record);
  }

  HP_GetAllEntries(*header_info, NULL);


  int val = 4999;
  HP_DeleteEntry(*header_info, &val);

  HP_GetAllEntries(*header_info, NULL);

  HP_CloseFile(header_info);

  fclose(ptr);

}

int main () {
  char* filename = "records5K.txt";
	void *block;
  read_records(filename);
}
