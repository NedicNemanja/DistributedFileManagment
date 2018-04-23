#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ReadPaths.h"
#include "ErrorCodes.h"
#include "Arguments.h"
#include "Piping.h"

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
    //resize the Paths array
    paths_size++;
    Paths = realloc(Paths,sizeof(char*)*paths_size);
    NULL_Check(Paths);
    //store the path at the end
    //not used: RemoveNewline(&line,temp);
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

void DistributePaths(pid_t* Children, char** Paths, int numPaths, int* OpenToPipes){
  //for a good DistributePaths, we need some quick mafs
  int load_size = ceil((double)numPaths/numWorkers);  //numPaths per load
  int Paths_index = 0;

  //make and send a load for every worker
  for(int i=0; i<numWorkers; i++){
    char* loadstr = MakeLoad(Paths,numPaths,&Paths_index,load_size);
    //send the load
    Send(Children[i],OpenToPipes[i],loadstr);
  }
}

char* MakeLoad(char** Paths, int numPaths,int* Paths_indexptr, int load_size){
  int Paths_index = *Paths_indexptr; //dereference for readability
  char* buffer = malloc(sizeof(char)*1);
  strcpy(buffer,"\0");
  size_t buffer_size = 1;

  for(int i=Paths_index; i<(Paths_index+load_size); i++){
    if(i>=numPaths) //dont get out of the paths array
      break;
    //add the strings
    buffer_size += sizeof(char)*strlen(Paths[i]);
    buffer = (char*)realloc(buffer,buffer_size);
    NULL_Check(buffer);
    strcat(buffer,Paths[i]);
    //move along in the paths array
    (*Paths_indexptr)++;
  }

  return buffer;
}
