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

  HT_CreateIndex(filename, 'i', "DDA", 4, 10);

  HT_info* header_info = HT_OpenIndex(filename);

  Record record;

  for (int i=0;i<800;i++) {
    if (i%2 == 0) {
      record.id = i;
      strcpy(record.name, "nicholas");
      strcpy(record.surname, "cage");
      strcpy(record.address, "sex");
    } else {
      record.id = i;
      strcpy(record.name, "holas");
      strcpy(record.surname, "cage");
      strcpy(record.address, "ex");
    }
    HT_InsertEntry(*header_info, record);
  }


  int dick = 5;
  printf("%d\n", HT_GetAllEntries(*header_info, &dick));

  printf("----------------------------------\n");

  int* ick = NULL;
  printf("%d\n", HT_GetAllEntries(*header_info, ick));
}
