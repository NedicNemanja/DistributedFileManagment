#ifndef WORKER_H
#define WORKER_H

int Worker();

void OpenWorkerPipes(int* to_pipe, int* from_pipe,int wrk_num);

char** DivideDirs(char* msg,int* numDirs);
void FreeDirs(char**);

char** GetDirFiles(char** Dirs, int numDirs, int* numFiles);
void FreeFilePaths(char**,int);

#endif
