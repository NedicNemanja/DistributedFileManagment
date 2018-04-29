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

//send answers 1 by 1 for each line (not used)
char* SendSearchAnswers( pid_t ppid, int from_pipe,
                    PostingList** Results, int numResults,
                    char** FilePaths, int numFiles,
                    DocumentMAP** DocMaps);

//send all answers at once
void SendSearchAnswer(pid_t ppid, int from_pipe,
                        Post*** PostsByFile, int* PostsInFile,
                        char** FilePaths, int numFiles,
                        DocumentMAP** DocMaps,FILE* log_fd);

/*Return the answer to /maxcount or /mincount to the parent
format: path+space+num */
void SendCountAnswer(pid_t ppid, int from_pipe, char* path, int num);

//send total_lines+total_bytes
void SendWcAnswer( pid_t ppid, int from_pipe, unsigned total_lines,
                                              unsigned int total_bytes);

#endif
