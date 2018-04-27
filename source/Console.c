#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "Arguments.h"
#include "Piping.h"
#include "Console.h"
#include "StringManipulation.h"
#include "ErrorCodes.h"
#include <poll.h>
#include "Querry.h"

void Console(pid_t* Children,int* OpenToPipes,int* OpenFromPipes){
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
      //read deadline
      int deadline;
      scanf("%d", &deadline);
      //ignore all until \n
      ReadTillNewline();
      //send the querry to the children
      char* question = malloc(sizeof(char)*
                              (strlen(command)+1+strlen(querry_str)+1));
      strcpy(question,command);
      strcat(question," ");
      strcat(question,querry_str);
      SendToAll(Children,OpenToPipes,question);
      free(question);
      //wait for all workers to answer
      //struct sigaction act;
      //SetAnswerSignal();  //count the child answers using signals

      int numAnswers = 0;
      while(numAnswers < numWorkers){
        //block until at least one pipe is available for read
        struct pollfd PollPipes[numWorkers];
        InitalizePipesPoll(PollPipes,OpenFromPipes);
      //  printf("poll bloc\n");
        int val = poll(PollPipes,numWorkers,-1);
        //printf("poll unbloc\n");

        //check which pipes are available for read and read from them
        for(int i=0; i<numWorkers; i++){
          if(PollPipes[i].revents == POLLIN){
            numAnswers++;
            char* msg = Receive(OpenFromPipes[i]);
            if(1/*systemtime < deadline*/){
              printf("%s", msg);
            }
            free(msg);
          }
          else if(PollPipes[i].revents == POLLERR)
            exit(PIPE_POLLERR);
        }
        //printf("out\n");
      }
      if(numAnswers < numWorkers){
        printf("%d/%d Workers answered in time.\n", numAnswers,numWorkers);
      }
      FreeQuerry(querry);
      free(querry_str);
      continue;
    }

    /**************************************************************************/
    else if(!strcmp(command,"/maxcount")){

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
    printf("loop\n");
    free(command);
  }while(1);
}

/**************Utility Functions***********************************************/

void InitalizePipesPoll(struct pollfd* PollPipes, int* OpenFromPipes){
  for(int i=0; i<numWorkers; i++){
    PollPipes[i].fd = OpenFromPipes[i];
    PollPipes[i].events = POLLIN; //only insterested when there is data to read
  }
}

/*
void SetAnswerSignal(struct sigaction* act){
  ANSWERS = 0;
  act->sa_handler = childAnswered;
  sigemptyset(act->sa_mask);
  adc->sa_flags = SA_SIGINFO;
  sigaction(SIGUSR1,act,NULL);
}


void childAnswered(siginfo_t* info){
  ANSWERS++;
  int i = (info->si_pid);
}*/

/*Read a word from stdin dynamically and return the char at the end of the wrd*/
char getWord(char** wordptr){
//  char c = SkipWhitespace(stdin);
char c = getchar();
  int size=0;
  while(c != '\n' && c != ' ' && c != '\t'){
    size++;
    *wordptr = realloc(*wordptr,sizeof(char)*(size+1));
    (*wordptr)[size-1] = c;
    c = getchar();
  }
  (*wordptr)[size] = '\0';
  return c;
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
