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

  printf("%d\n", header_info->fileDesc);

  printf("%c\n", header_info->attrType);

  printf("%s\n", header_info->attrName);

  printf("%d\n", header_info->attrLength);

}
