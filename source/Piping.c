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

void msg_signal(){
  signal(SIGUSR1, msg_signal);
  READ_FLAG += 2; //header+msg=2messages
}

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
  unsigned int msg_size = strlen(msg);
printf("Sending msg %zu:<<%s>>\n", strlen(msg),msg);
  //when breaking up the msg its important to know what's to be sent next
  unsigned int msg_offset = 0;

  while(msg_offset < msg_size){
    PipeHead header;
    //in case the message cant fit adjust its size
    int real_size;
    if(msg_size - msg_offset > MAX_MSG_SIZE-sizeof(int)){ //if msg can't fit
      //the msg is a part of a sequence
      header = (1 << (sizeof(int)*8-1) );
      real_size = MAX_MSG_SIZE;
    }
    else{                             //if the msg can fit
      //the last msg of the sequence has 0 as lmb at header
      header = 0;
      real_size = msg_size-msg_offset;
    }

    //send header
    header += real_size; //include the size in the header
    write(fd,&header,sizeof(int));
    //send message
    write(fd,msg+msg_offset,real_size+1);
    msg_offset += real_size;
  }
  //notify the receiver that he has a message
  kill(receiver,SIGUSR1);
  //msg sent, free it
  free(msg);
}

int READ_FLAG = 0;

char* Receive(int fd){
  char* msg = NULL;
  int msg_size = 0;
  if(READ_FLAG > 0){
    //read the header and get the msg size
    int header;
    read(fd,&header,sizeof(int));
    //if this is a sequence of messages wait until you get the whole sequence
    while(header < 0){
      //get one message in a buffer
      int buffer_size = header - (1 << (sizeof(int)*8-1) );
      char* buffer = malloc(sizeof(char)*buffer_size+1);
      NULL_Check(buffer);
      read(fd,buffer,buffer_size+1);
      //then concatenate the message with the buffer
      msg_size += buffer_size;
      msg = realloc(msg,sizeof(char)*msg_size+1);
      NULL_Check(msg);
      strcat(msg,buffer);
      free(buffer);
      READ_FLAG -= 2;
      //pause until you get a signal for the next message of the sequence
      pause();
      //read the new header
      int header;
      read(fd,&header,sizeof(int));
    }
    //this is the last message of the sequence
    if(msg_size>0){
      //get last message in buffer
      char* buffer = malloc(sizeof(char)*header+1);
      NULL_Check(buffer);
      read(fd,buffer,header+1);
      //then concatenate the message with the buffer
      msg_size += header;
      msg = realloc(msg,sizeof(char)*msg_size+1);
      NULL_Check(msg);
      strcat(msg,buffer);
      free(buffer);
      READ_FLAG -= 2;
    }
    else{ //there was only one message to begin with
      msg = malloc(sizeof(char)*header+1);
      read(fd,msg,header+1);
      READ_FLAG -= 2;
      printf("Received msg %zu:<<%s>>\n", strlen(msg),msg);
    }
  }
  return msg;
}

void SendToAll(pid_t* Children,int* OpenToPipes,char* msg){
  for(int i=0; i<numWorkers; i++){
    Send(Children[i],OpenToPipes[i],msg);
  }
}

int NumDigits(int i){
  if(i==0)
  return 1;
  /*do i+1 because in the case of i=10 log10(10)=1 and we need 2*/
  return (int)ceil(log10(i+1));
}
