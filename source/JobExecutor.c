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

  pid_t ogparent = getpid();
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
      exit(FORK_FAIL);
    }
    if(Children[i] == 0){ //this is the child
      printf("Child%d reporting %d.\n", i, getpid());
      return Worker(i);
    }
  }

  //This is the Parent**********************************

  /*do not receive notification when child processes  stop  (i.e.,  when  they
  receive one of SIGSTOP,SIGTSTP, SIGTTIN, or SIGTTOU) or resume (i.e.SIGCONT)
  +do not transform children into zombies*/
  struct sigaction saction;
  saction.sa_handler = SIG_IGN,
  sigemptyset(&saction.sa_mask);
  saction.sa_flags = SA_NOCLDSTOP | SA_NOCLDWAIT;
  sigaction(SIGCHLD,&saction,NULL);

  //if a pipe dies ignore the signal and try to restore it later
  struct sigaction saction1;
  saction1.sa_handler = SIG_IGN,
  sigemptyset(&saction1.sa_mask);
  saction1.sa_flags = 0;
  sigaction(SIGPIPE,&saction1,NULL);

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
  ERRORCODE err = Console(Children,OpenToPipes,OpenFromPipes,
                              Paths,numPaths);

  FreePaths(Paths,numPaths);
  //if this is the parent
  if(ogparent == getpid()){
    //wait for all children to terminate
    int status;
    while(wait(&status) > 0);

    UnlinkExecutorPipes(OpenToPipes,OpenFromPipes);
  }
  return err;
}
