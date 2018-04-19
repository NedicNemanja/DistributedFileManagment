#include <stdio.h>
#include <stdlib.h>
#include "ReadPaths.h"
#include "ErrorCodes.h"

char** ReadPaths(char* docfilename, int* numPathsptr){
  //open file for reading
  FILE* fp;
  fp = fopen(docfilename, "r");
  if(!fp){
    fprintf(stderr,"Can't open file %s", docfilename);
    ERROR = CANT_OPEN_FILE;
    return NULL;
  }

  //using an array of char* to store the paths
  char** Paths = NULL;
  int paths_size=0;

  //read line by line
  char *line = NULL;
  size_t len=1;
  ssize_t t=0;


  while( (t = getline(&line, &len, fp)) != -1){
    //resize the Paths array
    paths_size++;
    Paths = realloc(Paths,sizeof(char*)*paths_size);
    NULL_Check(Paths);
    //store the path at the end
    Paths[paths_size-1] = line;
    line = NULL;
  }
  free(line);
  //close the file
  fclose(fp);

  *numPathsptr = paths_size;
  return Paths;
}
