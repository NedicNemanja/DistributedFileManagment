#include "Piping.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include "ErrorCodes.h"
#include "Arguments.h"
#include "StringManipulation.h"

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

  return pipename;
}

void OpenExecutorPipes(int* OpenToPipes, int* OpenFromPipes){
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
}

void OpenWorkerPipes(int* to_pipe, int* from_pipe, int wrk_num){
  char* to_pipename = PipeName("to",wrk_num);
  char* from_pipename = PipeName("from",wrk_num);
  if(((*to_pipe) = open(to_pipename, O_RDONLY)) < 0){
    perror("fifo open\n");
    exit(1);
  }
  if(((*from_pipe) = open(from_pipename, O_WRONLY)) < 0){
    perror("fifo open\n");
    exit(1);
  }
  free(to_pipename);
  free(from_pipename);
}

void Send(pid_t receiver, int fd, char* msg){
  unsigned int msg_size = strlen(msg)+1;
  //when breaking up the msg its important to know what's to be sent next
  unsigned int msg_offset = 0;

  while(msg_offset < msg_size){
    PipeHead header;
    //in case the message cant fit adjust its size
    int real_size;
    if(msg_size - msg_offset > MAX_MSG_SIZE){ //if msg can't fit
      //the msg is a part of a sequence
      header = (1 << (sizeof(int)*8-1) );
      real_size = MAX_MSG_SIZE; //-1 cause we have to fit the \0 also
    }
    else{                             //if the msg can fit (<=MAX_MSG_SIZE)
      //the last msg of the sequence has 0 as lmb at header
      header = 0;
      real_size = msg_size-msg_offset;
    }

    //send header
    header += real_size; //include the size in the header
    write(fd,&header,sizeof(int));
    printf("sent header:%u\n",header);
    //send message
    write(fd,msg+msg_offset,real_size);
    msg_offset += real_size;
  }
//  printf("Sending msg %zu:\n<<%s>>\n", strlen(msg),msg);
}

char* Receive(int fd){
  char* msg = malloc(sizeof(char));
  msg[0] = '\0';
  int msg_size = 0;
    //read the header and get the msg size
    int header;
    read(fd,&header,sizeof(int));
    printf("received header(%d):%u\n",(header & (1 << (sizeof(int)*8-1))),header);
    //if this is a sequence of messages wait until you get the whole sequence
    while( (header & (1 << (sizeof(int)*8-1))) ){ //while lmb != 0
      //get one message in a buffer
      int buffer_size = header - (1 << (sizeof(int)*8-1) );
      char* buffer = malloc(sizeof(char)*(buffer_size+1));
      NULL_Check(buffer);
      read(fd,buffer,buffer_size);
      buffer[buffer_size] = '\0';
      //then concatenate the message with the buffer
      msg_size += buffer_size;
      msg = realloc(msg,sizeof(char)*(msg_size+1));
      NULL_Check(msg);
      strcat(msg,buffer);
      free(buffer);
      //read the new header
      read(fd,&header,sizeof(int));
      printf("received header(%d):%u\n",(header & (1 << (sizeof(int)*8-1))),header);
    }

    //this is the last message of the sequence
    if(msg_size>0){
      //get last message in buffer
      char* buffer = malloc(sizeof(char)*(header+1));
      NULL_Check(buffer);
      read(fd,buffer,header);
      buffer[header] = '\0';
      //then concatenate the message with the buffer
      msg_size += header;
      msg = realloc(msg,sizeof(char)*(msg_size+1));
      NULL_Check(msg);
      strcat(msg,buffer);
      free(buffer);
    }
    else{ //there was only one message to begin with
      msg = malloc(sizeof(char)*(header+1));
      msg_size = header;
      NULL_Check(msg);
      read(fd,msg,msg_size);
    }
//  printf("Received msg %zu:\n<<%s>>\n", strlen(msg),msg);
  if(msg_size == 0)
    return NULL;
  return msg;
}

void SendToAll(pid_t* Children,int* OpenToPipes,char* msg){
  for(int i=0; i<numWorkers; i++){
    Send(Children[i],OpenToPipes[i],msg);
  }
}
