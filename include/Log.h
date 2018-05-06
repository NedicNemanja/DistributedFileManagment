#ifndef LOG_H
#define LOG_H
#include <stdio.h>
#include "Querry.h"
#include "PostingList.h"

FILE* OpenLog();

char* GetTime();

/*Write a partial log for a /search*/
void WriteLogSearch(FILE* fd, char* timestr, char* qtype, Querry* querry,
                                      PostingList** Results, char** FilePaths);

void WriteLogMaxcount(FILE* fd, char* timestr, char* qtype, char* keyword,
                                                          char* filepath);

void WriteLogMincount(FILE* fd, char* timestr, char* qtype, char* keyword,
                                                          char* filepath);

void WriteLogWc(FILE* fd, char* timestr, char* qtype, unsigned int lines,
                                                      unsigned int words,
                                            unsigned long long int bytes);
#endif
