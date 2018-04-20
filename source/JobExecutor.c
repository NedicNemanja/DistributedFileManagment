#include <stdio.h>
#include "Arguments.h"
#include "ReadPaths.h"
#include "ErrorCodes.h"

int main(int argc, char* argv[]){
  ReadArguments(argc,argv);
  
  int numPaths;
  char** Paths = ReadPaths(docfilename,&numPaths);
  for(int i=0; i<numPaths; i++){
    printf("%s", Paths[i]);
    FILE* f = fopen(Paths[i],"r");
    if(f != NULL){
      char c = fgetc(f);
      printf("read:%c\n", c);
      fclose(f);
    }
    else
      perror("cant open");
  }

  FreePaths(Paths,numPaths);
  return 0;
}
