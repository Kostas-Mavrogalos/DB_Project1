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

  char sad[3] = "\",";

  Record record;
  fscanf(ptr, "{%s}", buffer);

  char* bugger;

HT_CreateIndex("ht_file", 'i', "DDA", 4, 10);
  HT_info* header_info = HT_OpenIndex("ht_file");

  while (fscanf(ptr, "\n{%s}", buffer) != EOF) {
    bugger = strtok(buffer, ",\"");
    record.id = atoi(bugger);
    printf("%d\n", record.id);
    strcpy(record.name, strtok(NULL, sad));
    printf("%s\n", record.name);
    strcpy(record.surname, strtok(NULL, sad));
    printf("%s\n", record.surname);
    strcpy(record.address, strtok(NULL, sad));
    printf("%s\n", record.address);
    HT_InsertEntry(*header_info, record);
  }

  HT_GetAllEntries(*header_info, NULL);

  fclose(ptr);

}

int main () {
  char* filename = "records10K.txt";
	void *block;
  read_records(filename);

  HashStatistics("ht_file");
}
