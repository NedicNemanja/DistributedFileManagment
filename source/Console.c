#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "Arguments.h"
#include "Piping.h"
#include "Console.h"
#include "StringManipulation.h"
#include "ErrorCodes.h"

void Console(pid_t* Children,int* OpenToPipes,int* OpenFromPipes){
  printf("Console open:\n");
  char* command = NULL;
  do{
    char c = getWord(&command);

    /**************************************************************************/
    if(!strcmp(command,"/search")){
      //read querry
      char* querry = ReadQuerry();
      if(querry == NULL) continue;
      //read deadline
      int deadline;
      scanf("%d", &deadline);
      //ignore all until \n
      ReadTillNewline();
      //send the qurry to the children
      strcat(command,querry);
      SendToAll(Children,OpenToPipes,command);
      //wait for all workers to answer
      //struct sigaction act;
      //SetAnswerSignal();  //count the child answers using signals

      int numAnswers = 0;
      while(numAnswers < numWorkers){
        //block until at least one pipe is available for read
        struct pollfd PollPipes[numWorkers];
        InitalizePipesPoll(&PollPipes,OpenFromPipes);
        int val = poll(PollPipes,numWorkers,-1);
        //check which pipes are available for read and read from them
        for(int i=0; i<numWorkers; i++){
          if(PollPipes[i]->revent == POLLIN){
            numAnswers++;
            char* msg = Receive(OpenFromPipes[i]);
            if(systemtime < deadline){
              printf("%s", msg);
            }
          }
          else if(PollPipes[i]->revent == POLLERR)
            exit(PIPE_POLLERR);
        }
      }
      if(numAnswers < numWorkers){
        printf("%d/%d Workers answered in time.\n", numAnswers,numWorkers);
      }
      PrintAnswers();
      continue;
    }

    /**************************************************************************/
    if(!strcmp(command,"/maxcount")){

      continue;
    }

    /**************************************************************************/
    if(!strcmp(command,"/mincount")){

      continue;
    }
    /**************************************************************************/
    if(!strcmp(command,"/wc")){

      continue;
    }
    /**************************************************************************/
    if(!strcmp(command,"/exit")){

      continue;
    }

  }while(strcmp(command,"/exit"));
  free(command);
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
  char c = SkipWhitespace(stdin);
  int size=0;
  while(c != '\n' && c != ' ' && c != '\t'){
    size++;
    *wordptr = realloc(*wordptr,sizeof(char)*(size+1));
    (*wordptr)[size-1] = c;
    c = getc(stdin);
  }
  (*wordptr)[size] = '\0';
  return c;
}

char* ReadQuerry(){
  char* querry=NULL;
  int qsize=0;
  //read until you find "-d"
  char c, prev_c='.';
  c = getchar();
  while(c != 'd' && prev_c != '-' && c!='\n'){
    qsize++;
    querry = realloc(querry,sizeof(char)*qsize);
    NULL_Check(querry);
    querry[qsize-1] = c;
    c = getchar();
  }
  //remove the - and null terminate the string
  if(prev_c == '-'){
    querry[qsize-1] = '\0';
  }
  else{
    querry = realloc(querry,sizeof(char)*qsize+1);
    NULL_Check(querry);
    querry[qsize] = '\0';
  }
  //exceptions
  if(c=='\n'){
    printf("Please specify deadline. Correct syntax: /search querry -d 1.123\n");
    return NULL;
  }
  if(querry==NULL){
    printf("No querry found. Correct syntax: /search querry -d deadline\n");
  }
  return querry;
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
