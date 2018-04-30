#include "Querry.h"
#include <stdlib.h>
#include <string.h>
#include "ErrorCodes.h"

Querry* CreateQuerry(char* msg){
  //ignore 1st word (instruction)
  int msg_index = 0;
  while(msg[msg_index] != ' '){
    msg_index++;
  }
  msg_index++;
  //create and initalize
  Querry* querry = malloc(sizeof(Querry));
  NULL_Check(querry);
  querry->q = NULL;
  querry->size = 0;
  //get all the words until you find "-d"
  char* word;
  do{
    //get next word
    word = NULL;
    int word_size = 0;
    while(msg[msg_index] != ' ' && msg[msg_index]!='\0' && msg[msg_index]!='\n'){
      word_size++;
      word = realloc(word,sizeof(char)*(word_size+1));
      NULL_Check(word);
      word[word_size-1] = msg[msg_index];
      word[word_size] = '\0';
      msg_index++;
    }
    if(msg[msg_index] !='\0' && msg[msg_index] !='\n')
      msg_index++;
    if(word == NULL){
      break;
    }
    //insert word in q array
    if(strcmp(word,"-d")){
      //resize array
      querry->size++;
      querry->q = realloc(querry->q,sizeof(char*)*(querry->size));
      NULL_Check(querry->q);
      //insert word
      querry->q[querry->size-1] = word;
    }
  }while(strcmp(word,"-d") && msg[msg_index]!='\0' && msg[msg_index]!='\n');
  return querry;
}

Querry* CreateQuerryStdin(){
  Querry* querry = malloc(sizeof(Querry));
  NULL_Check(querry);
  querry->q = NULL;
  querry->size = 0;
  //read
  //get all the words until you find "-d"
  char* word;
  char c;
  do{
    //get next word
    word = NULL;
    int word_size = 0;
    c = getchar();
    while(c != ' ' && c!='\n'){
      word_size++;
      word = realloc(word,sizeof(char)*(word_size+1));
      NULL_Check(word);
      word[word_size-1] = c;
      c = getchar();
    }
    word[word_size] = '\0';
    //insert word in q array
    if(strcmp(word,"-d") && c!='\n'){
      //resize array
      querry->size++;
      querry->q = realloc(querry->q,sizeof(char*)*(querry->size));
      NULL_Check(querry->q);
      //insert word
      querry->q[querry->size-1] = word;
    }
  }while(strcmp(word,"-d") && c !='\n');
  //exceptions
  if(c=='\n'){
    free(querry);
    free(word);
    printf("Please specify deadline. Correct syntax: /search querry -d 1.123\n");
    return NULL;
  }
  if(querry->q == NULL){
    free(querry);
    free(word);
    printf("No querry found. Correct syntax: /search querry -d deadline\n");
    return NULL;
  }
  free(word);
  return querry;
}

void FreeQuerry(Querry* querry){
  for(unsigned int i=0; i<(querry->size); i++){
    free(querry->q[i]);
  }
  free(querry->q);
  free(querry);
}

char* QuerryToStr(Querry* querry,const char* delimeter){
  //allocate adequate space for the sting,
  //to fit all the words and a space after each word
  int str_size=0;
  for(int i=0; i<querry->size; i++){
    str_size += strlen(querry->q[i])+1;
  }
  char* str = malloc(sizeof(char)*str_size);
  NULL_Check(str);
  str[0] = '\0';
  //add words
  for(int i=0; i<querry->size; i++){
    strcat(str,querry->q[i]);
    if(i != querry->size-1)
      strcat(str,delimeter);
  }
  return str;
}
