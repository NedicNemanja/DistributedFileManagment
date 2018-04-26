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

int Worker(int wrk_num){
  pid_t ppid =getppid();
  //open pipes from child side
  int to_pipe,from_pipe;
  OpenWorkerPipes(&to_pipe,&from_pipe,wrk_num);
  //pipes ready
  //read dirs when you receive SIGUSR1,its the 1st sequence of msg you get
  signal(SIGUSR1, msg_signal);
  pause();  //wait for a message
  char* msg = Receive(to_pipe);
  //parse directories and get their files
  int numDirs,numFiles;
  char** Dirs = DivideDirs(msg,&numDirs);
  char** FilePaths = GetDirFiles(Dirs,numDirs,&numFiles);
  FreeDirs(Dirs);
  //load files to memory and return their maps
  DocumentMAP** DocMaps = LoadFiles(FilePaths,numFiles);
  //ready to receive instructions from parent
  do{
    char* msg = Receive(to_pipe);
    char* instruction = getInstruction(msg);
    if(!strcmp(command,"/search"){
      //get the posting lists for the querry
      int numResults;
      PostingList** Results = Search(msg,&numResults);
      char* answer = MakeAnswer(Results,numResults);
      Send(ppid,from_pipe,answer);
      continue;
    }

  }while(instruction != "/exit")


  FreeFilePaths(FilePaths,numFiles);
  FreeDocMaps(DocMaps,numFiles);
  free(msg);
  return 0;
}

/*Find and return all the PostingLists of every word in the msg*/
void Search(char* msg, int numResults){
  Querry* querry = CreateQuerry(msg);

  //find all the posting lists
  PostingList** Results = malloc(sizeof(PostingList*)*(querry->size));
  for(int i=0; i<querry->size; i++){
    //if alarm deadline break
    Results[i] = SearchTrie(querry->q[i],TrieRoot,1);
  }
  FreeQuerry(querry);

  *numResults = querry->size;
  return Results;
}

char* MakeAnswer( PostingList** Results, int numResults,
                  char** FilePaths, int numFiles,
                  DocumentMAP** DocMaps){
  //for every posting list group its posts by file_id
  Post*** PostsByFile = calloc(sizeof(Post**)*numFiles); //2d array of Post* grouped by file_id
  int* PostsInFile = calloc(sizeof(int)*numFiles);  //number of posts per file
  for(int i=0; i<numResults; i++){
    //get every post of this file
    GroupByFile(PostsByFile, PostsInFile, PostingList* Results[i]);
  }

  char* answer = "\0";
  int answer_size = 0;
  //for every file add its results in the answer
  for(int i=0; i<numFiles; i++){
        //include file path in the answer
        answer_size += strlen(FilePaths[i])+2;
        answer = realloc(sizeof(char)*(answer_size+1));
        NULL_Check(answer);
        sprintf(answer, "%s:\n", FilePaths[i]);
        //include all the documents and their doc_id's (line & linenumbers)
        for(int k=0; i<PostsInFile; j++){
          Post* post = PostsByFile[i][j];
          answer_size +=  NumDigits(post->doc_id)+2+
                          strlen(DocMaps[i][post->doc_id])+1;
          answer = realloc(sizeof(char)*(answer_size+1));
          NULL_Check(answer);
          sprintf(answer, "%d: %s\n", post->doc_id,DocMaps[i][post->doc_id]);
        }
  }
  //freepostsbyfile and freepostsinfile
  for(int i=0; i<numFiles; i++){
    free(PostsByFile[i]);
  }
  free(PostsByFile);
  free(PostsInFile);

  return answer;
}

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
