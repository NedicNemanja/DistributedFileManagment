#ifndef WORKER_H
#define WORKER_H

#include "PostingList.h"
#include "DocumentMap.h"

int Worker();

//sets the DEADLINE flag to 1 once the deadline is up
void deadline_handler(int signum);

void OpenWorkerPipes(int* to_pipe, int* from_pipe,int wrk_num);

char** DivideDirs(char* msg,int* numDirs);
void FreeDirs(char**);

char** GetDirFiles(char** Dirs, int numDirs, int* numFiles);
void FreeFilePaths(char**,int);

/*Find and return all the pointer to PostingLists of every word in the msg*/
PostingList** Search(char* msg, int* numResults);

//send answers 1 by 1 for each line
char* SendSearchAnswers( pid_t ppid, int from_pipe,
                    PostingList** Results, int numResults,
                    char** FilePaths, int numFiles,
                    DocumentMAP** DocMaps);

//send all answers at once
void SendSearchAnswer(pid_t ppid, int from_pipe,
                        PostingList** Results, int numResults,
                        char** FilePaths, int numFiles,
                        DocumentMAP** DocMaps);

#endif
