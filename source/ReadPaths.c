#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include "ReadPaths.h"
#include "ErrorCodes.h"
#include "Arguments.h"
#include "Piping.h"
#include "StringManipulation.h"

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
    if(loadstr == NULL)
      break;
    //send the load
    Send(Children[i],OpenToPipes[i],loadstr);
    free(loadstr);
  }
}

char* MakeLoad(char** Paths, int numPaths,int* Paths_indexptr, int load_size){
  int Paths_index = *Paths_indexptr; //dereference for readability
  char* buffer = malloc(sizeof(char));
  strcpy(buffer,"\0");
  int buffer_size = 0;

  for(int i=Paths_index; i<(Paths_index+load_size); i++){
    if(i>=numPaths) //dont get out of the paths array
      break;
    //add the strings
    buffer_size += strlen(Paths[i]);
    buffer = (char*)realloc(buffer,sizeof(char)*(strlen(Paths[i])+1));
    NULL_Check(buffer);
    strcat(buffer,Paths[i]);
    //move along in the paths array
    (*Paths_indexptr)++;
  }
  if(buffer_size == 0){
    free(buffer);
    return NULL;
  }
  return buffer;
}

void DistributePath(pid_t* Children, char** Paths, int numPaths, int* OpenToPipes,int wrk_num){
  //for a good DistributePaths, we need some quick mafs
  int load_size = ceil((double)numPaths/numWorkers);  //numPaths per load
  int Paths_index = 0;

  //make and send a load for every worker
  for(int i=0; i<numWorkers; i++){
    char* loadstr = MakeLoad(Paths,numPaths,&Paths_index,load_size);
    //send the load only to the selected worker, ignore others
    if(wrk_num == i)
      Send(Children[i],OpenToPipes[i],loadstr);
    free(loadstr);
  }
}



char** DivideDirs(char* msg,int* numDirs){
  char** Dirs = NULL;
  *numDirs = 0;
  int offset=0;
  //break up the string into tokens and store each token in Dirs
  char* token = getWordStr(msg);
  while(token != NULL){
    printf("token:%s\n", token);
    offset += strlen(token);
    (*numDirs)++;
    Dirs = realloc(Dirs,sizeof(char*)*(*numDirs));
    NULL_Check(Dirs);
    Dirs[(*numDirs)-1] = token;
    token =  getWordStr(msg+offset);
  }

  return Dirs;
}

void FreeDirs(char** Dirs,int numDirs){
  for(int i=0; i<numDirs; i++){
    free(Dirs[i]);
  }
  free(Dirs);
}

char** GetDirFiles(char** Dirs, int numDirs, int* numFiles){
  char** Files = NULL;
  *numFiles = 0;

  for(int i=0; i<numDirs; i++){
    //open directory
    DIR* dir = opendir(Dirs[i]);
    if(dir == NULL){
      perror("Cant open dir\n");
      exit(CANT_OPEN_DIR);
    }
    //read files  from directory
    struct dirent* file;
    while( (file = readdir(dir)) != NULL){
      //ignore current(.) and parent(..) dirs
      if((strcmp(file->d_name,".")==0) || (strcmp(file->d_name,"..")==0))
        continue;
      //resize Files array
      (*numFiles)++;
      Files = realloc(Files,sizeof(char*)*(*numFiles));
      NULL_Check(Files);
      //copy the filename to the array
      Files[(*numFiles)-1] = malloc(sizeof(char)*(strlen(Dirs[i])+1+
                                                  strlen(file->d_name)+1));
      NULL_Check(Files[(*numFiles)-1]);
      strcpy(Files[(*numFiles)-1],Dirs[i]);
      strcat(Files[(*numFiles)-1],"/");
      strcat(Files[(*numFiles)-1],file->d_name);
    }
    //close directory
    if(closedir(dir) != 0){
      perror("Failed to close dir\n");
      exit(CANT_CLOSE_DIR);
    }
  }

  return Files;
}

void FreeFilePaths(char** FilePaths, int numFiles){
  for(int i=0; i<numFiles; i++){
    free(FilePaths[i]);
    FilePaths[i] = NULL;
  }
  free(FilePaths);
}
