#ifndef CONSOLE_H
#define CONSOLE_H

#include <sys/types.h>
#include <poll.h>
#include <signal.h>
#include "ErrorCodes.h"

ERRORCODE Console(pid_t* Children,int* OpenToPipes,int* OpenFromPipes,
                                char** Paths, int numPaths);

//Check if all children are still alive, revive these that are dead
ERRORCODE CheckChildren(pid_t* Children, int* OpenToPipes, int* OpenFromPipes,
                                char** Paths, int numPaths);

/*Initalize Answers array to NULL and wait using poll() for all children to
answer. Return the Answers array*/
void GetAllAnswers(char* Answers[], int* OpenFromPipes);


//Used to init all parent pipes for use in poll for POLLIN
void InitalizePipesPoll(struct pollfd* PollPipes, int* OpenFromPipes);
//Used to init Answers array to NULL
void InitalizeArray(char** Answers);


//DEADLINE
void alrm_handler(int signum);
void SetDeadlineAlarm(unsigned int deadline);


/*****************Print answers to stdin***************************/
//Print and free answers for search
void PrintSearch(char** Answers);
//For /maxcount find the max Answer and print it, frees all answers
void PrintMaxCount(char** Answers);
//For /mincount find the min Answer and print it, frees all answers
void PrintMinCount(char** Answers);
//For /wc
void PrintWc(char** Answers);

#endif
