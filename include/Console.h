#ifndef CONSOLE_H
#define CONSOLE_H

#include <sys/types.h>


void Console(pid_t* Children,int* OpenToPipes,int* OpenFromPipes);

/*Read from stdin until you find "-d" and return what you read.
On fail return NULL*/
char* ReadQuerry();

/*Read a word from stdin dynamically and return the char at the end of the wrd*/
char getWord(char** wordptr);

/*Read till the end of the current stdin stream (terminates with \n)*/
void ReadTillNewline();

void PrintAnswers();

#endif
