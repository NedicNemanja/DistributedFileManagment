#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "Arguments.h"
#include "ReadPaths.h"
#include "Piping.h"
#include "ErrorCodes.h"
#include "Worker.h"
#include "Console.h"

int main(int argc, char* argv[]){
  ReadArguments(argc,argv);

  //start workers
  pid_t Children[numWorkers];
  for(int i=0; i<numWorkers; i++){
    //make a pipe jobexe->workeri
    if(MakePipePair(i) != 0){
      perror("pipe make");
      exit(1);
    }
    //make a worker
    Children[i] = fork();
    if(Children[i] == -1){
      perror("fork fail\n");
      exit(2);
    }
    if(Children[i] == 0){ //this is the child
      printf("Child%d reporting %d.\n", i, getpid());
      return Worker(i);
    }
  }

  //This is the Parent**********************************
  //open pipes from parent side
  int OpenToPipes[numWorkers];
  int OpenFromPipes[numWorkers];
  OpenExecutorPipes(OpenToPipes,OpenFromPipes);

  //Read the directories paths
  int numPaths;
  char** Paths = ReadPaths(docfilename,&numPaths);
  //Distribute the paths to the workers
  DistributePaths(Children,Paths,numPaths,OpenToPipes);

  //open console for user input
  Console(Children,OpenToPipes,OpenFromPipes);

  //wait for all children to terminate
  int status;
  while(wait(&status) > 0);
//unlink pipes
  FreePaths(Paths,numPaths);

  return 0;
}
