#ifndef WORKER_H
#define WORKER_H

#include "PostingList.h"
#include "DocumentMap.h"

int Worker();

void OpenWorkerPipes(int* to_pipe, int* from_pipe,int wrk_num);

char** DivideDirs(char* msg,int* numDirs);
void FreeDirs(char**);

char** GetDirFiles(char** Dirs, int numDirs, int* numFiles);
void FreeFilePaths(char**,int);

/*Construct answers for the parent when asked to /search a querry.
For every file that had a line that matched the Querry with at least one word
return the path of the file followed by all the lines doc_id's followed by the
contents of the line.
ALl of this is put in a single string that is to be Send()*/
char* MakeSearchAnswer( PostingList** Results, int numResults,
                  char** FilePaths, int numFiles,
                  DocumentMAP** DocMaps);
#endif
