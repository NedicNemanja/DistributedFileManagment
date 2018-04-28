#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
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

int DEADLINE;

int Worker(int wrk_num){
  pid_t ppid =getppid();
  //open pipes from child side
  int to_pipe,from_pipe;
  OpenWorkerPipes(&to_pipe,&from_pipe,wrk_num);
  //pipes ready
  //block until you receive a message sequence (these will be the dirs)
  char* msg = Receive(to_pipe);
  //parse directories and get their files
  int numDirs,numFiles;
  char** Dirs = DivideDirs(msg,&numDirs);
  char** FilePaths = GetDirFiles(Dirs,numDirs,&numFiles);
  FreeDirs(Dirs);
  //load files to memory and return their maps
  DocumentMAP** DocMaps = LoadFiles(FilePaths,numFiles);
  //ready to receive instructions from parent
  char* instruction = NULL;
  do{
    char* msg = Receive(to_pipe);
    instruction = getWordStr(msg);

    if(!strcmp(instruction,"/search")){/***************************************/
      //set deadline, do not send answer after deadline is over
      DEADLINE=0;
      signal(SIGUSR1,deadline_handler);
      //get the posting lists for the querry
      int numResults=0;
      PostingList** Results = Search(msg,&numResults);
      sleep(10);
      //answer for all files at once
      SendSearchAnswer(ppid,from_pipe,
                        Results,numResults,FilePaths,numFiles,DocMaps);
      continue;
    }
    else if(!strcmp(instruction,"/maxcount")){/********************************/
      //get 1st word after instruction
      char* keyword = getWordStr(msg+10);
      //get the posting list of keyword
      PostingList* pl = SearchTrie(keyword,TrieNode,1);
      //find the post with the maxcount and return its file_id
      int file_id = MaxCountPost(pl);
      //send the file path as answer
      Send(ppid,from_pipe,FilePaths[i]);
    }

  }while(instruction != "/exit");


  FreeFilePaths(FilePaths,numFiles);
  FreeDocMaps(DocMaps,numFiles);
  free(msg);
  return 0;
}

void deadline_handler(int signum){
  DEADLINE = 1;
}


PostingList** Search(char* msg, int* numResults){
  Querry* querry = CreateQuerry(msg);

  //find all the posting lists
  PostingList** Results = malloc(sizeof(PostingList*)*(querry->size));
  for(int i=0; i<querry->size; i++){
    //if alarm deadline break
    PostingList* res = SearchTrie(querry->q[i],TrieRoot,1);
    if(res != NULL){  //if found
      Results[i] = res;
      (*numResults)++;
    }
  }

  FreeQuerry(querry);
  return Results;
}

int MaxCountFile(PostingList* pl,char** FilePaths){
  int max=-1;
  int max_file_id;
  for(int i=0; i<pl->doc_frequency; i++){
    Post* post = getPost(pl,i);
    if(post->recurrence > max)
      max_file_id = post->file_id;
    else if(post->recurrence = max){  //on equal choose aphabetically
      if(StrCompare(FilePaths[post->file_id],FilePaths[max_file_id]) == -1 ){
        
      }
    }
  }
  return max_file_id;
}

/*cmp 2 string alphabetically
if st1>str2 return 1
if str1=str2 return 0
if str1<str2 return -1*/
int StrCompare(char* str1,char* str2){

}

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

//send all answers at once
void SendSearchAnswer(pid_t ppid, int from_pipe,
                        PostingList** Results, int numResults,
                        char** FilePaths, int numFiles,
                        DocumentMAP** DocMaps){
  if(numResults == 0){  //if no results are found a empty string is sent to ppid
    Send(ppid,from_pipe,"\0");
    return;
  }
  //for every posting list group its posts by file_id
  Post*** PostsByFile = calloc(numFiles,sizeof(Post**)); //2d array of Post* grouped by file_id
  int* PostsInFile = calloc(numFiles,sizeof(int));  //number of posts per file
  for(int i=0; i<numResults; i++){
    //get every post of this file
    GroupByFile(PostsByFile, PostsInFile, Results[i]);
  }

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
  //freepostsbyfile and freepostsinfile
  for(int i=0; i<numFiles; i++){
    free(PostsByFile[i]);
  }
  free(PostsByFile);
  free(PostsInFile);
}

/*
char* MakeSearchAnswer( PostingList** Results, int numResults,
                  char** FilePaths, int numFiles,
                  DocumentMAP** DocMaps){
  //for every posting list group its posts by file_id
  Post*** PostsByFile = calloc(numFiles,sizeof(Post**)); //2d array of Post* grouped by file_id
  int* PostsInFile = calloc(numFiles,sizeof(int));  //number of posts per file
  for(int i=0; i<numResults; i++){
    //get every post of this file
    GroupByFile(PostsByFile, PostsInFile, Results[i]);
  }

  char* answer = "\0";
  int answer_size = 0;
  //for every file add its results in the answer
  for(int i=0; i<numFiles; i++){
        //include file path in the answer
        answer_size += strlen(FilePaths[i])+2;
        answer = realloc(answer,sizeof(char)*(answer_size+1));
        NULL_Check(answer);
        sprintf(answer, "%s:\n", FilePaths[i]);
        //include all the documents and their doc_id's (line & linenumbers)
        for(int j=0; j<PostsInFile[i]; j++){
          Post* post = PostsByFile[i][j];
          answer_size +=  NumDigits(post->doc_id)+2+
                          strlen(DocMaps[i]->map[post->doc_id])+1;
          answer = realloc(answer,sizeof(char)*(answer_size+1));
          NULL_Check(answer);
          sprintf(answer, "%d: %s\n", post->doc_id,DocMaps[i]->map[post->doc_id]);
        }
  }
  //freepostsbyfile and freepostsinfile
  for(int i=0; i<numFiles; i++){
    free(PostsByFile[i]);
  }
  free(PostsByFile);
  free(PostsInFile);

  return answer;
}*/

char** DivideDirs(char* msg,int* numDirs){
  char** Dirs = NULL;
  *numDirs = 0;
  int offset=0;
  //break up the string into tokens and store each token in Dirs
  char* token = strtok(msg,"\n");
  while(token != NULL){
    (*numDirs)++;
    Dirs = realloc(Dirs,sizeof(char*)*(*numDirs));
    NULL_Check(Dirs);
    Dirs[(*numDirs)-1] = token;
    token =  strtok(NULL,"\n");
  }

  return Dirs;
}

void FreeDirs(char** Dirs){
  free(Dirs);
}

char** GetDirFiles(char** Dirs, int numDirs, int* numFiles){
  char** Files = NULL;
  *numFiles = 0;

  for(int i=0; i<numDirs; i++){
    //open directory
    DIR* dir = opendir(Dirs[i]);
    if(dir == NULL){
      perror("Cant open dir\n");
      exit(CANT_OPEN_DIR);
    }
    //read files  from directory
    struct dirent* file;
    while( (file = readdir(dir)) != NULL){
      //ignore current(.) and parent(..) dirs
      if((strcmp(file->d_name,".")==0) || (strcmp(file->d_name,"..")==0))
        continue;
      //resize Files array
      (*numFiles)++;
      Files = realloc(Files,sizeof(char*)*(*numFiles));
      NULL_Check(Files);
      //copy the filename to the array
      Files[(*numFiles)-1] = malloc(sizeof(char)*(strlen(Dirs[i])+1+
                                                  strlen(file->d_name)+1));
      NULL_Check(Files[(*numFiles)-1]);
      strcpy(Files[(*numFiles)-1],Dirs[i]);
      strcat(Files[(*numFiles)-1],"/");
      strcat(Files[(*numFiles)-1],file->d_name);
    }
    //close directory
    if(closedir(dir) != 0){
      perror("Failed to close dir\n");
      exit(CANT_CLOSE_DIR);
    }
  }

  return Files;
}

void FreeFilePaths(char** FilePaths, int numFiles){
  for(int i=0; i<numFiles; i++){
    free(FilePaths[i]);
    FilePaths[i] = NULL;
  }
  free(FilePaths);
}
