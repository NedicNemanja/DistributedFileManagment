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
  signal(SIGUSR1,msg_signal);
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
      //ignore
      ReadTillNewline();
      //send the qurry to the children
      strcat(command,querry);
      SendToAll(Children,OpenToPipes,command);
      //wait for the workers to answer
      //sleep(deadline);
      if(READ_FLAG/2 < numWorkers){
        printf("%d/%d Workers answered in time.\n", READ_FLAG/2,numWorkers);
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
