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
  ssize_t temp=0;


  while( (temp = getline(&line, &len, fp)) != -1){
    printf("Read line of size %zu:%s", temp,line);
    //resize the Paths array
    paths_size++;
    Paths = realloc(Paths,sizeof(char*)*paths_size);
    NULL_Check(Paths);
    //store the path at the end
    RemoveNewline(&line,temp);
    Paths[paths_size-1] = line;
    line = NULL;
  }
  free(line);
  //close the file
  fclose(fp);

  *numPathsptr = paths_size;
  return Paths;
}

void FreePaths(char** Paths,int size){
  //free contents
  for(int i=0; i<size; i++){
    free(Paths[i]);
    Paths[i] = NULL;
  }
  //free the array
  free(Paths);
}

void RemoveNewline(char** lineptr,ssize_t size){
  //resize to cut the old \0
  *lineptr = realloc(*lineptr,sizeof(char)*size);
  //establish new \0 where \n used to be
  (*lineptr)[size-1] = '\0';
}
