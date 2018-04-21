#include "Piping.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ErrorCodes.h"

int MakePipePair(int i){
  char* pipename_to = PipeName("to",i);
  char* pipename_from = PipeName("from",i);
  if( mkfifo(pipename_to,0666) == -1){
    free(pipename_to);
    free(pipename_from);
    return -1;
  }
  if( mkfifo(pipename_from,0666) == -1){
    free(pipename_to);
    free(pipename_from);
    return -1;
  }
  free(pipename_to);
  free(pipename_from);

  return 0;
}

char* PipeName(const char* str, int i){
  char* pipename = malloc(sizeof(char)*(NumDigits(i)+strlen(str)+strlen(PIPE_DIR)+1));
  NULL_Check(pipename);
  //dir where the pipe is put
  strcpy(pipename,PIPE_DIR);
  //to or from
  strcat(pipename,str);
  //serial num i
  char* buffer = malloc(sizeof(char)*(NumDigits(i)+1));
  NULL_Check(buffer);
  sprintf(buffer,"%d",i);
  strcat(pipename,buffer);
  free(buffer);

  printf("pipename%s\n", pipename);
  return pipename;
}

int NumDigits(int i){
  if(i==0)
    return 1;
  /*we do i+1 because in the case of i=10 log10(10)=1 and we need 2*/
  return (int)ceil(log10(i+1));
}
