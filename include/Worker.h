#ifndef WORKER_H
#define WORKER_H

int Worker();

void OpenChildPipes(int* to_pipe, int* from_pipe,int wrk_num);

char** DivideDirs(char* msg,int* numDirs);

char** GetDirFiles(char** Dirs, int numDirs, int* numFiles);

#endif
