#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "Piping.h"
#include "Worker.h"
#include "ErrorCodes.h"
#include "LoadFile.h"
#include "DocumentMap.h"

int Worker(int wrk_num){
  //open pipes from child side
  int to_pipe,from_pipe;
  OpenWorkerPipes(&to_pipe,&from_pipe,wrk_num);
  //pipes ready
  //read dirs when you receive SIGUSR1,its the 1st sequence of msg you get
  signal(SIGUSR1, msg_signal);
  pause();  //wait for a message
  char* msg = Receive(to_pipe);
  //parse directories and get their files
  int numDirs,numFiles;
  char** Dirs = DivideDirs(msg,&numDirs);
  char** FilePaths = GetDirFiles(Dirs,numDirs,&numFiles);
  FreeDirs(Dirs);
  //load files to memory and return their maps
  DocumentMAP** DocMaps = LoadFiles(FilePaths,numFiles);

  for(int i=0; i<numFiles; i++){
    printf("***************************************************\n");
    printf("DocumentMAP of: %s\n", FilePaths[i]);
    if(DocMaps[i] == NULL) continue;
    PrintMAP(*(DocMaps[i]));
  }


  FreeFilePaths(FilePaths,numFiles);
  FreeDocMaps(DocMaps,numFiles);
  free(msg);
  return 0;
}

char** DivideDirs(char* msg,int* numDirs){
  char** Dirs = NULL;
  *numDirs = 0;
  int offset=0;
  //break up the string into tokens and store each token in Dirs
  char* token = strtok(msg,"\n");
  while(token != NULL){
    (*numDirs)++;
    Dirs = realloc(Dirs,sizeof(char*)*(*numDirs));
    NULL_Check(Dirs);
    Dirs[(*numDirs)-1] = token;
    token =  strtok(NULL,"\n");
  }

  return Dirs;
}

void FreeDirs(char** Dirs){
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
