#include "Log.h"
#include "ErrorCodes.h"
#include "StringManipulation.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "PostingList.h"
#include "Post.h"

FILE* OpenLog(){
  int pid = (int)getpid();
  int buffer_size = NumDigits(pid);
  char* buffer = malloc(sizeof(char)*(buffer_size+1));
  NULL_Check(buffer);
  sprintf(buffer, "%d", pid);

  char* filename = malloc(sizeof(char)*(13+buffer_size+1));
  NULL_Check(filename);
  strcpy(filename,"./log/Worker_");
  strcat(filename,buffer);
  free(buffer);

  FILE* fd = fopen(filename,"w+");
  free(filename);
  if(fd == NULL)
    exit(CANT_OPEN_FILE);
  return fd;
}

char* GetTime(){
  time_t t;
  struct tm* timeinfo;
  time(&t);
  timeinfo = localtime(&t);
  char* str = malloc(sizeof(char)*26);
  NULL_Check(str);
  sprintf(str, "%s", asctime(timeinfo));
  str[24] = '\0';
  str = realloc(str,sizeof(char)*25);
  return str;
}

/*Write a partial log for a /search*/
void WriteLogSearch(FILE* fd, char* timestr, char* qtype, Querry* querry,
                                      PostingList** Results, char** FilePaths){
  int res_index=-1;
  //for every word of querry that was found
  for(int i=0; i<querry->size; i++){
    if(querry->q[i] != NULL){
      res_index++;
      fprintf(fd, "%s:%s:%s:", timestr,qtype,querry->q[i]);
      //add the results (filepaths)
      for(int j=0; j<Results[res_index]->doc_frequency; j++){
        Post* post = getPost(Results[res_index],j);
        fprintf(fd, "%s:", FilePaths[post->file_id]);
      }
      fprintf(fd, "\n");
    }
  }
}

void WriteLogMaxcount(FILE* fd, char* timestr, char* qtype, char* keyword,
                                                          char* filepath){
  fprintf(fd, "%s:%s:%s:%s\n", timestr,qtype,keyword,filepath);
}

void WriteLogMincount(FILE* fd, char* timestr, char* qtype, char* keyword,
                                                          char* filepath){
  fprintf(fd, "%s:%s:%s:%s\n", timestr,qtype,keyword,filepath);
}

void WriteLogWc(FILE* fd, char* timestr, char* qtype, unsigned int lines,
                                                      unsigned int words,
                                            unsigned long long int bytes){
  char* bytesstr = malloc(sizeof(char)*(NumDigits(bytes)+1));
  sprintf(bytesstr, "%u", (unsigned int)bytes);
  char* wordsstr = malloc(sizeof(char)*(NumDigits(words)+1));
  sprintf(wordsstr, "%u", words);
  char* linesstr = malloc(sizeof(char)*(NumDigits(lines)+1));
  sprintf(linesstr, "%u", lines);
  fprintf(fd, "%s:%s:%s %s %s\n", timestr,qtype,linesstr,wordsstr,bytesstr);
  free(bytesstr);
  free(wordsstr);
  free(linesstr);
}
