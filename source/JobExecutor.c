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
    fopen(Paths[i],"r");
  }

  fprintf(stderr, "%d\n", ERROR);
  return 0;
}
