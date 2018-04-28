#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "Arguments.h"
#include "Piping.h"
#include "Console.h"
#include "StringManipulation.h"
#include "ErrorCodes.h"
#include <sys/poll.h>
#include <fcntl.h>
#include "Querry.h"
#include <unistd.h>

int DEADLINE;    //1 if deadline is up, 0 otherwise
pid_t* CHILDREN; //used to send deadline signal to all children

void Console(pid_t* Children,int* OpenToPipes,int* OpenFromPipes){
  CHILDREN = Children;
  printf("Console open:\n");
  do{
    char* command = NULL;
    char c = getWord(&command);

    /**************************************************************************/
    if(!strcmp(command,"/search")){
      //read querry
      Querry* querry = CreateQuerryStdin();
      if(querry == NULL) continue;
      char* querry_str = QuerryToStr(querry);
      //read and set deadline
      unsigned int deadline;
      scanf("%u", &deadline);
      SetDeadlineAlarm(deadline);
      //ignore all until \n
      ReadTillNewline();
      //send the querry to the children
      char* question = malloc(sizeof(char)*
                              (7+1+strlen(querry_str)+1));
      strcpy(question,command);
      strcat(question," ");
      strcat(question,querry_str);
      SendToAll(Children,OpenToPipes,question);
      free(question);
      //wait for all workers to answer

      int numAnswers = 0; //num of workers that answered in time
      char* Answers[numWorkers];
      while(numAnswers < numWorkers && DEADLINE!=1){
        //block until at least one pipe is available for read or a signal arrives
        struct pollfd PollPipes[numWorkers];
        InitalizePipesPoll(PollPipes,OpenFromPipes);
        printf("poll bloc\n");
        int val = poll(PollPipes,(nfds_t)numWorkers,-1);
        printf("poll unbloc\n");

        //check which pipes are available for read from them
        for(int i=0; i<numWorkers; i++){
          {
            if(PollPipes[i].revents == POLLIN){
              char* msg = Receive(OpenFromPipes[i]);
              numAnswers++;
              if(DEADLINE != 1)
                Answers[i] = msg;
            }
            else if(PollPipes[i].revents == POLLERR)
              exit(PIPE_POLLERR);
          }
        }
      }
      if(numAnswers < numWorkers){
        printf("%d/%d Workers answered in time.\n", numAnswers,numWorkers);
      }
      //print answers
      for(int i=0; i<numWorkers; i++){
        if(Answers[i] != NULL){
          printf("%s", Answers[i]);
          free(Answers[i]);
        }
      }
      FreeQuerry(querry);
      free(querry_str);
      free(command);
      continue;
    }

    /**************************************************************************/
    else if(!strcmp(command,"/maxcount")){
      //send the question to all children
      char* keyword;
      c = getWord(&keyword);
      char* question = malloc(sizeof(char)*(9+1+strlen(keyword)+1));
      strcpy(question,command);
      strcat(question," ");
      strcat(question,keyword);
      SendToAll(Children,OpenToPipes,question);
      free(question);
      //wait for children to answer

      continue;
    }

    /**************************************************************************/
    else if(!strcmp(command,"/mincount")){

      continue;
    }
    /**************************************************************************/
    else if(!strcmp(command,"/wc")){

      continue;
    }
    /**************************************************************************/
    else if(!strcmp(command,"/exit")){
      //kill children
      printf("Bye bye!\n");
      break;
    }
    else{
      printf("%s unknown command.\n", command);
    }
  }while(1);
}

/**************Utility Functions***********************************************/

void InitalizePipesPoll(struct pollfd* PollPipes, int* OpenFromPipes){
  for(int i=0; i<numWorkers; i++){
    PollPipes[i].fd = OpenFromPipes[i];
    PollPipes[i].events = POLLIN; //only insterested when there is data to read
    PollPipes[i].revents = 0;
  }
}

void alrm_handler(int signum){
  printf("---DEADLINE---\n");
  DEADLINE = 1;
  //tell children deadline is up so they stop answering to the related command
  for(int i=0; i<numWorkers; i++){
    kill(CHILDREN[i],SIGUSR1);
  }
}

void SetDeadlineAlarm(unsigned int deadline){
  DEADLINE = 0; //alrm_handler will set this to 1 when deadline expires
  //set up 1 SIGALRM in deadline seconds
  alarm(deadline);
  //set up a signal handler for SIGALRM
  signal(SIGALRM,alrm_handler);
}

/*Read till the end of the current stdin stream (terminates with \n)*/
void ReadTillNewline(){
  char c;
  do{
    c = getc(stdin);
  }while(c != '\n');
}

void PrintAnswers(){
  printf("PrintAnswers not implemented yet.\n");
}
