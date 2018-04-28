#ifndef CONSOLE_H
#define CONSOLE_H

#include <sys/types.h>
#include <poll.h>
#include <signal.h>

void Console(pid_t* Children,int* OpenToPipes,int* OpenFromPipes);

/*Read till the end of the current stdin stream (terminates with \n)*/
void ReadTillNewline();

void PrintAnswers();

//Used to init all parent pipes for use in poll for POLLIN
void InitalizePipesPoll(struct pollfd* PollPipes, int* OpenFromPipes);


void alrm_handler(int signum);
void SetDeadlineAlarm(unsigned int deadline);

#endif
