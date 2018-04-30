#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>
#include "Arguments.h"
#include "Piping.h"
#include "Console.h"
#include "StringManipulation.h"
#include "ErrorCodes.h"
#include "Querry.h"
#include "Worker.h"
#include "ReadPaths.h"


volatile sig_atomic_t DEADLINE;    //1 if deadline is up, 0 otherwise
pid_t* CHILDREN; //used to send deadline signal to all children

ERRORCODE Console(pid_t* Children,int* OpenToPipes,int* OpenFromPipes,
                        char** Paths, int numPaths){
  CHILDREN = Children;
  printf("Console open:\n");
  do{
    //CHECK IF CHILDREN STILL ALIVE HERE
    ERRORCODE check = CheckChildren(Children,OpenToPipes,OpenFromPipes,
                                                        Paths,numPaths);
    if(check != THIS_IS_PARENT)
      //if this is not the parent we dont want the process to continue
      return check;
    //Children may live but the pipes could be broken
    //CheckPipes(OpenToPipes,OpenFromPipes);

    char* command;
    char c = getWord(&command);
    while(command == NULL)
      c = getWord(&command); //dont accept empty input
    /**************************************************************************/
    if(!strcmp(command,"/search")){
      //read querry
      Querry* querry = CreateQuerryStdin();
      if(querry == NULL){
        free(command);
        continue;
      }
      char* querry_str = QuerryToStr(querry," ");
      //read and set deadline
      unsigned int deadline;
      scanf("%u", &deadline);
      SetDeadlineAlarm(deadline);
      ReadTillNewline();
      //send the querry to the children
      char* question = malloc(sizeof(char)*(7+1+strlen(querry_str)+1));
      strcpy(question,command);
      strcat(question," ");
      strcat(question,querry_str);
      if(SendToAll(Children,OpenToPipes,question) != OK){
        alarm(0);
        fprintf(stderr, "Parent found one or more unresponsive children.\n");
        printf("Try again.\n");
        free(question);
        FreeQuerry(querry);
        free(querry_str);
        free(command);
        continue;
      }
      free(question);
      //wait for all workers to answer
      char* Answers[numWorkers];
      GetAllAnswers(Answers,OpenFromPipes);
      //if theres an alarm for a deadline, cancel it
      alarm(0);
      DEADLINE = 0;
      //print and free Answers
      PrintSearch(Answers);
      FreeQuerry(querry);
      free(querry_str);
      free(command);
      continue;
    }

    /**************************************************************************/
    else if(!strcmp(command,"/maxcount")){
      //send the question to all children
      char* keyword = NULL;
      c = getWord(&keyword);
      //ignore until newline
      if(c != '\n')
        ReadTillNewline();
      //make question
      char* question = malloc(sizeof(char)*(9+1+strlen(keyword)+1));
      strcpy(question,command);
      strcat(question," ");
      strcat(question,keyword);
      if(SendToAll(Children,OpenToPipes,question) != OK){
        fprintf(stderr, "Parent found one or more unresponsive children.\n");
        printf("Try again.\n");
        free(question);
        free(keyword);
        free(command);
        continue;
      }
      free(keyword);
      free(question);
      //wait for all workers to answer
      char* Answers[numWorkers];
      GetAllAnswers(Answers,OpenFromPipes);
      //print and free answers
      PrintMaxCount(Answers);
      free(command);
      continue;
    }

    /**************************************************************************/
    else if(!strcmp(command,"/mincount")){
      //send the question to all children
      char* keyword=NULL;
      c = getWord(&keyword);
      //ignore until newline
      if(c != '\n')
        ReadTillNewline();
      //make question
      char* question = malloc(sizeof(char)*(9+1+strlen(keyword)+1));
      strcpy(question,command);
      strcat(question," ");
      strcat(question,keyword);
      if(SendToAll(Children,OpenToPipes,question) != OK){
        fprintf(stderr, "Parent found one or more unresponsive children.\n");
        printf("Try again.\n");
        free(question);
        free(keyword);
        free(command);
        continue;
      }
      free(keyword);
      free(question);
      //wait for all workers to answer
      char* Answers[numWorkers];
      GetAllAnswers(Answers,OpenFromPipes);
      //print and free answers
      PrintMinCount(Answers);
      free(command);
      continue;
    }
    /**************************************************************************/
    else if(!strcmp(command,"/wc")){
      //ignore until newline
      if(c != '\n')
        ReadTillNewline();
      //send the /wc command to all children
      if(SendToAll(Children,OpenToPipes,command) != OK){
        fprintf(stderr, "Parent found one or more unresponsive children.\n");
        printf("Try again.\n");
        free(command);
        continue;
      }
      //wait for all workers to answer
      char* Answers[numWorkers];
      GetAllAnswers(Answers,OpenFromPipes);
      //print and free answers
      PrintWc(Answers);
      free(command);
      continue;
    }
    /**************************************************************************/
    else if(!strcmp(command,"/exit")){
      SendToAll(Children,OpenToPipes,command);
      printf("Bye bye!\n");
      free(command);
      break;
    }
    else{
      printf("%s unknown command.\n", command);
      ReadTillNewline();
      free(command);
    }
  }while(1);

  return OK;
}

/**************Utility Functions***********************************************/

ERRORCODE CheckChildren(pid_t* Children, int* OpenToPipes, int* OpenFromPipes,
                              char** Paths, int numPaths){
//printf("CHecking children\n");
int val;
  //check every child
  for(int i=0; i<numWorkers; i++){
    //if a child is dead revive it
    if((val = waitpid(Children[i],NULL,WNOHANG)) != 0){
      //printf("Child%d died.Reset pipes.\n", val);
      //reset executor pipes
      UnlinkExecutorPipe(OpenToPipes,OpenFromPipes, i);
      if(MakePipePair(i) != 0){
        perror("pipe make");
        exit(CANT_MAKE_PIPE);
      }
      //revive
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
      //this is the parent
      OpenExecutorPipe(OpenToPipes,OpenFromPipes, i);
      DistributePath(Children,Paths,numPaths,OpenToPipes, i);
      //printf("Child%d restored.\n", i);
    }
  }
//printf("CHeck over%d.\n", val);
  return THIS_IS_PARENT;
}

void GetAllAnswers(char* Answers[], int* OpenFromPipes){
  int numAnswers = 0; //num of workers that answered in time
  int numNonResposive = 0;  //num of workers that were found non responsive
  InitalizeArray(Answers);
  do{
    //block until at least one pipe is available for read
    struct pollfd PollPipes[numWorkers];
    InitalizePipesPoll(PollPipes,OpenFromPipes);
    fprintf(stderr, "poll bloc\n");
    int val = poll(PollPipes,(nfds_t)numWorkers,-1);
    fprintf(stderr,"poll unbloc\n");

    //check which pipes are available for read and read from them
    for(int i=0; i<numWorkers; i++){
      {
        if(PollPipes[i].revents == POLLIN){
          char* msg = Receive(OpenFromPipes[i]);
          numAnswers++;
          if(DEADLINE != 1)
            Answers[i] = msg;
          else{
            Answers[i] = NULL;
            free(msg);
          }
        }
        else if(PollPipes[i].revents == POLLHUP ||
                PollPipes[i].revents == POLLERR ||
                PollPipes[i].revents == POLLNVAL){
          //unable to get answer form this child (chld dead or broken pipe)
          Answers[i] = NULL;
          numNonResposive++;
        }
      }
    }
  }while(numAnswers+numNonResposive < numWorkers);
  if(numAnswers < numWorkers){
    printf("%d/%d Workers answered in time.\n", numAnswers,numWorkers);
  }
}

void InitalizePipesPoll(struct pollfd* PollPipes, int* OpenFromPipes){
  for(int i=0; i<numWorkers; i++){
    PollPipes[i].fd = OpenFromPipes[i];
    PollPipes[i].events = POLLIN; //only insterested when there is data to read
    PollPipes[i].revents = 0;
  }
}

void InitalizeArray(char** Answers){
  for(int i=0; i<numWorkers; i++){
    Answers[i] = NULL;
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

void PrintSearch(char** Answers){
  for(int i=0; i<numWorkers; i++){
    if(Answers[i] != NULL){
      printf("%s", Answers[i]);
      free(Answers[i]);
    }
  }
}

void PrintMaxCount(char** Answers){
  int max_answer = -1;
  int max_answer_pos=-1;
  //find the max answer
  for(int i=0; i<numWorkers; i++){
    if(Answers[i][0] != '\0' && Answers[i]!=NULL){
      //get the file path
      char* path = getWordStr(Answers[i]);
      //get the number of recurrence in this file
      char* num_start_ptr = Answers[i]+strlen(path)+1;
      int num = (int)strtol(num_start_ptr,NULL,10);
      //keep track of max_answer
      if((int)num > max_answer){
        max_answer = num;
        max_answer_pos = i;
      }
      else if((int)num == max_answer){
        //on equal choose by alphabetical priority
        if(mystrcmp(Answers[i],Answers[max_answer_pos]) == -1){
          max_answer = num;
          max_answer_pos = i;
        }
      }
      free(path);
    }
  }
  if(max_answer_pos != -1)
    printf("%s\n", Answers[max_answer_pos]);
  else
    printf("keyword not found in any file.\n");
  //free answers
  for(int i=0; i<numWorkers; i++){
    if(Answers[i]!=NULL)
      free(Answers[i]);
  }
}

void PrintMinCount(char** Answers){
  int min_answer = INT_MAX;
  int min_answer_pos=-1;
  //find the max answer
  for(int i=0; i<numWorkers; i++){
    if(Answers[i][0] != '\0' && Answers[i]!=NULL){
      //get the file path
      char* path = getWordStr(Answers[i]);
      //get the number of recurrence in this file
      char* num_start_ptr = Answers[i]+strlen(path)+1;
      int num = (int)strtol(num_start_ptr,NULL,10);
      //keep track of min_answer
      if((int)num < min_answer){
        min_answer = num;
        min_answer_pos = i;
      }
      else if((int)num == min_answer){
        //on equal choose by alphabetical priority
        if(mystrcmp(Answers[i],Answers[min_answer_pos]) == -1){
          min_answer = num;
          min_answer_pos = i;
        }
      }
      free(path);
    }
  }
  if(min_answer_pos != -1)
    printf("%s\n", Answers[min_answer_pos]);
  else
    printf("keyword not found in any file.\n");
  //free answers
  for(int i=0; i<numWorkers; i++){
    if(Answers[i]!=NULL)
      free(Answers[i]);
  }
}

void PrintWc(char** Answers){
  int total_lines=0;
  int total_bytes=0;
  for(int i=0; i<numWorkers; i++){
    if(Answers[i] != NULL){
      total_lines += (int)strtol(Answers[i],NULL,10);
      total_bytes += (int)strtol(Answers[i]+NumDigits(total_lines)+1,NULL,10);
    }
    else{
      printf("Worker%d failed to respond. Cant calculate wc.\n", i);
      return;
    }
    free(Answers[i]);
  }
  printf("total number of lines:%d\ntotal number of bytes:%d\n",
                                        total_lines,total_bytes);
}
