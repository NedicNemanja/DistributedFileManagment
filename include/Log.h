#ifndef LOG_H
#define LOG_H
#include <stdio.h>

FILE* OpenLog();

char* GetTime();

/*Write a partial log for a /search,
filepaths are written by WriteLogSearchFilepath*/
void WriteLogSearch(FILE* fd, char* timestr, char* qtype, char* query);

void WriteLogSearchFilepath(FILE* fd, char* filepath);

void WriteLogMaxcount(FILE* fd, char* timestr, char* qtype, char* keyword,
                                                          char* filepath);

void WriteLogMincount(FILE* fd, char* timestr, char* qtype, char* keyword,
                                                          char* filepath);

void WriteLogWc(FILE* fd, char* timestr, char* qtype, unsigned int lines,
                                            unsigned long long int bytes);
#endif
