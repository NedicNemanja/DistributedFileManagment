#include <stdio.h>
#include "Arguments.h"
#include "ReadPaths.h"
#include "Piping.h"
#include "ErrorCodes.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

int MakePipePair(int i);

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

  //start workers
  pid_t Children[numWorkers];
  for(int i=0; i<numWorkers; i++){
    //make a pipe jobexe->workeri
    if(MakePipePair(i) != 0){
      perror("pipe make");
      exit(1);
    }
    else
      printf("made pipes %i\n\n", i);
    //make a worker
    Children[i] = fork();
    if(Children[i] == -1){
      perror("fork fail\n");
      exit(2);
    }
    if(Children[i] == 0){ //this is the child
      printf("Child%d reporting %d.\n", i, getpid());
      //open pipes from child side
      int to_pipe,from_pipe;
      char* to_pipename = PipeName("to",i);
      char* from_pipename = PipeName("from",i);
      if((to_pipe = open(to_pipename, O_RDONLY)) < 0){
        perror("fifo open\n");
        exit(1);
      }
      if((from_pipe = open(from_pipename, O_WRONLY)) < 0){
        perror("fifo open\n");
        exit(1);
      }
      free(to_pipename);
      free(from_pipename);
      FreePaths(Paths,numPaths);
      printf("-------------%d---------\n", getpid());
      return 0;
    }
  }
  sleep(10);

  //This is the Parent
  //open pipes from parent side
  int OpenToPipes[numWorkers];
  int OpenFromPipes[numWorkers];
  for(int i=0; i<numWorkers; i++){
    char* to_pipename = PipeName("to",i);
    char* from_pipename = PipeName("from",i);
    if((OpenToPipes[i] = open(to_pipename, O_WRONLY)) < 0){
      perror("fifo open\n");
      exit(1);
    }
    if((OpenFromPipes[i] = open(from_pipename, O_RDONLY)) < 0){
      perror("fifo open\n");
      exit(1);
    }
    free(to_pipename);
    free(from_pipename);
  }
sleep(20);
  FreePaths(Paths,numPaths);
  return 0;
}
