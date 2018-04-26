#include "Querry.h"
#include <stdlib.h>
#include <sting.h>
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
  do{
    //get next word
    char* word = NULL;
    int word_size = 0;
    while(msg[msg_index] != ' '){
      word_size++;
      word = realloc(word,sizeof(char)*word_size);
      NULL_Check(word);
      msg_index++;
    }
    //insert word in q array
    if(strcmp(word,"-d") && strcmp(word,"\n")){
      querry->size++;
      querry->q[querry->size-1] = malloc(sizeof(char*));
      NULL_Check(querry->q);
      querry->q[querry->size-1] = word;
    }
  }while(strcmp(word,"-d") && strcmp(word,"\n"));

  return querry;
}

void FreeQuerry(Querry* querry){
  for(int i=0; i<querry->size; i++){
    free(querry->q[i]);
  }
  free(querry);
}
