#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Piping.h"
#include "Worker.h"
#include "ErrorCodes.h"
#include "LoadFile.h"
#include "DocumentMap.h"
#include "StringManipulation.h"
#include "Trie.h"
#include "PostingList.h"
#include "Querry.h"
#include "Log.h"
#include "ReadPaths.h"

volatile sig_atomic_t DEADLINE; //deadline flag,triggered by SIGUSR1

int Worker(int wrk_num){
  signal(SIGPIPE,SIG_DFL);
  //open/create log file
  FILE* log_fd = OpenLog();
  pid_t ppid = getppid();
  //open pipes from child side
  int to_pipe,from_pipe;
  OpenWorkerPipes(&to_pipe,&from_pipe,wrk_num);
  //pipes ready
  //block until you receive a message sequence (these will be the dirs)
  char* dirmsg = Receive(to_pipe);
  //parse directories and get their files
  int numDirs,numFiles;
  char** Dirs = DivideDirs(dirmsg,&numDirs);
  free(dirmsg);
  char** FilePaths = GetDirFiles(Dirs,numDirs,&numFiles);
  FreeDirs(Dirs,numDirs);
  //load files to memory and return their maps
  DocumentMAP** DocMaps = LoadFiles(FilePaths,numFiles);
  //ready to receive instructions from parent
  while(1){
    char* msg = Receive(to_pipe);
    //note time of query
    char* curr_time = GetTime();

    char* instruction = NULL;
    instruction = getWordStr(msg);

    if(!strcmp(instruction,"/search")){/***************************************/
      //set deadline, do not send answer after deadline is over
      DEADLINE=0;
      signal(SIGUSR1,deadline_handler);
      //get the posting lists for the querry
      Querry* querry = CreateQuerry(msg);
      int numResults=0;
      PostingList** Results = SearchTrieQuerry(querry,&numResults);
      //log
      WriteLogSearch(log_fd,curr_time,instruction,QuerryToStr(querry," "));
      //if no results are found a empty string is sent to ppid
      if(numResults == 0){
        Send(ppid,from_pipe,"\0");
      }
      else{
        //Group All posts from Results by file in a 2d array of Post*
        int* PostsInFile;
        Post*** PostsByFile = GroupAllByFile(Results,numResults,numFiles,
                                                    &PostsInFile);
        //answer for all files at once
        SendSearchAnswer( ppid,from_pipe, PostsByFile,PostsInFile,
                                          FilePaths,numFiles,DocMaps,log_fd);
      }
      //reset SIGUSR1
      signal(SIGUSR1,SIG_DFL);
      //clean up
      FreeQuerry(querry);
      free(Results);
      free(instruction);
      free(msg);
      free(curr_time);
      continue;
    }
    else if(!strcmp(instruction,"/maxcount")){/********************************/
      //get 1st word after instruction
      char* keyword = getWordStr(msg+10);
      //get the posting list of keyword
      PostingList* pl = SearchTrie(keyword,TrieRoot,1);
      if(pl != NULL){
        //find the post with the maxcount and return its file_id
        int max;
        int file_id = MaxRecurrenceFile(pl,FilePaths,&max);
        //send the file path+max as answer
        SendCountAnswer(ppid,from_pipe,FilePaths[file_id],max);
        //log
        WriteLogMaxcount(log_fd,curr_time,instruction,keyword,
                                            FilePaths[file_id]);
      }
      else{
        //keyword not found
        Send(ppid,from_pipe,"\0");
        //log
        WriteLogMaxcount(log_fd,curr_time,instruction,keyword,"not_found");
      }
      //clean up
      free(keyword);
      free(instruction);
      free(msg);
      free(curr_time);
      continue;
    }
    else if(!strcmp(instruction,"/mincount")){/********************************/
      //get 1st word after instruction
      char* keyword = getWordStr(msg+10);
      //get the posting list of keyword
      PostingList* pl = SearchTrie(keyword,TrieRoot,1);
      if(pl != NULL){
        //find the post with the maxcount and return its file_id
        int min;
        int file_id = MinRecurrenceFile(pl,FilePaths,&min);
        //send the file path+num as answer
        SendCountAnswer(ppid,from_pipe,FilePaths[file_id],min);
        //log
        WriteLogMaxcount(log_fd,curr_time,instruction,keyword,
                                            FilePaths[file_id]);
      }
      else{
        //keyword not found
        Send(ppid,from_pipe,"\0");
        //log
        WriteLogMaxcount(log_fd,curr_time,instruction,keyword,"not_found");
      }
      //clean up
      free(keyword);
      free(instruction);
      free(msg);
      free(curr_time);
      continue;
    }
    else if(!strcmp(instruction,"/wc")){/**************************************/
      unsigned int total_lines = GetNumLines(DocMaps,numFiles);
      SendWcAnswer(ppid,from_pipe,total_lines,TOTAL_SIZE);
      //log
      WriteLogWc(log_fd,curr_time,instruction,total_lines,TOTAL_SIZE);
      //clean up
      free(instruction);
      free(msg);
      free(curr_time);
      continue;
    }
    else if(!strcmp(instruction,"/exit")){/************************************/
      //clean up
      free(instruction);
      free(msg);
      free(curr_time);
      break;
    }
  }

  FreeFilePaths(FilePaths,numFiles);
  FreeDocMaps(DocMaps,numFiles);
  FreeTrie(TrieRoot);
  FreeList(PLIST.next);
  close(to_pipe);
  close(from_pipe);
  fclose(log_fd);
  return 0;
}

void deadline_handler(int signum){
  DEADLINE = 1;
}

//send all answers at once and log the filepaths
void SendSearchAnswer(pid_t ppid, int from_pipe,
                        Post*** PostsByFile, int* PostsInFile,
                        char** FilePaths, int numFiles,
                        DocumentMAP** DocMaps, FILE* log_fd){

  char* total_answer = malloc(sizeof(char));
  total_answer[0] = '\0';
  int total_answer_size = 0;
  //get an answer for every file
  for(int i=0; i<numFiles; i++){
        //include file path in the answer
        int filepath_str_size = strlen(FilePaths[i])+1;
        char* filepath_str = malloc(sizeof(char)*(filepath_str_size+1));
        NULL_Check(filepath_str);
        sprintf(filepath_str, "%s:", FilePaths[i]);
        //write log if there are posts linked with this file
        if(PostsInFile[i]>0){
          WriteLogSearchFilepath(log_fd, filepath_str);
        }

        //get an answer for each line
        for(int j=0; j<PostsInFile[i]; j++){
          Post* post = PostsByFile[i][j];
          //get doc_id: linestring
          int doc_str_size = NumDigits(post->doc_id)+3+
                              strlen(DocMaps[i]->map[post->doc_id])+1;
          char* doc_str = malloc(sizeof(char)*(doc_str_size+1));
          sprintf(doc_str,"%d:\n %s\n", post->doc_id, DocMaps[i]->map[post->doc_id]);
          //concatenate filepath_str+doc_str to make answer
          int answer_size = filepath_str_size + doc_str_size;
          char* answer = malloc(sizeof(char)*(answer_size+1));
          NULL_Check(answer);
          strcpy(answer,filepath_str);
          strcat(answer,doc_str);
          //concatenate answer for this line to total_answer
          total_answer_size += answer_size;
          total_answer=realloc(total_answer,sizeof(char)*(total_answer_size+1));
          strcat(total_answer,answer);
          free(doc_str);
          free(answer);
        }
        free(filepath_str);
  }

  if(DEADLINE != 1)
    Send(ppid,from_pipe,total_answer);
  free(total_answer);
  //freepostsbyfile and freepostsinfile
  for(int i=0; i<numFiles; i++){
    free(PostsByFile[i]);
  }
  free(PostsByFile);
  free(PostsInFile);
}

void SendCountAnswer(pid_t ppid, int from_pipe, char* path, int num){
  //num to str
  int numstr_len = NumDigits(num);
  char* numstr = malloc(sizeof(char)*(numstr_len+1));
  sprintf(numstr, "%d", num);
  //glue path+ +num
  char* answer = malloc(sizeof(char)*(strlen(path)+1+numstr_len+1));
  strcpy(answer,path);
  strcat(answer," ");
  strcat(answer,numstr);
  //send
  Send(ppid,from_pipe,answer);
  //clean up
  free(numstr);
  free(answer);
}

void SendWcAnswer( pid_t ppid, int from_pipe, unsigned total_lines,
                                              unsigned int total_bytes){
  int answer_size = NumDigits(total_lines)+1+NumDigits(total_bytes)+1;
  char* answer = malloc(sizeof(char)*answer_size);
  NULL_Check(answer);
  sprintf(answer,"%d %d",total_lines,total_bytes);
  Send(ppid,from_pipe,answer);
  free(answer);
}

/**************FILES AND DIRS***********************************************/

//send answers 1 by 1 for each line
char* SendSearchAnswers(pid_t ppid, int from_pipe,
                        PostingList** Results, int numResults,
                        char** FilePaths, int numFiles,
                        DocumentMAP** DocMaps){
  //for every posting list group its posts by file_id
  Post*** PostsByFile = calloc(numFiles,sizeof(Post**)); //2d array of Post* grouped by file_id
  int* PostsInFile = calloc(numFiles,sizeof(int));  //number of posts per file
  for(int i=0; i<numResults; i++){
    //get every post of this file
    GroupByFile(PostsByFile, PostsInFile, Results[i]);
  }

  //send an answer for every file
  for(int i=0; i<numFiles; i++){
        //include file path in the answer
        int filepath_str_size = strlen(FilePaths[i])+1;
        char* filepath_str = malloc(sizeof(char)*(filepath_str_size+1));
        NULL_Check(filepath_str);
        sprintf(filepath_str, "%s:", FilePaths[i]);

        //send an answer for each line
        for(int j=0; j<PostsInFile[i]; j++){
          Post* post = PostsByFile[i][j];
          //get doc_id: linestring
          int doc_str_size = NumDigits(post->doc_id)+2+
                          strlen(DocMaps[i]->map[post->doc_id])+1;
          char* doc_str = malloc(sizeof(char)*(doc_str_size+1));
          sprintf(doc_str,"%d: %s\n", post->doc_id, DocMaps[i]->map[post->doc_id]);
          //concatenate filepath_str+doc_str to make answer
          int answer_size = filepath_str_size + doc_str_size;
          char* answer = malloc(sizeof(char)*(answer_size+1));
          NULL_Check(answer);
          strcpy(answer,filepath_str);
          strcat(answer,doc_str);
          //send answer for this line
          Send(ppid,from_pipe,answer);
          free(doc_str);
          free(answer);
        }
        free(filepath_str);
  }
  //freepostsbyfile and freepostsinfile
  for(int i=0; i<numFiles; i++){
    free(PostsByFile[i]);
  }
  free(PostsByFile);
  free(PostsInFile);
}
