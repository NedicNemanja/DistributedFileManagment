#include <stdio.h>
#include <stdlib.h>
#include "Word.h"
#include "ErrorCodes.h"

Word* CreateWord(int f_id,int d_id,int word_size,int word_start,char* w_ptr){
  Word* new_word = malloc(sizeof(Word));
  NULL_Check(new_word);
  //INITALIZE
  new_word->file_id = f_id;
  new_word->doc_id = d_id;
  new_word->size = word_size;
  new_word->start = word_start;
  new_word->wordptr = w_ptr;

  return new_word;
}

Word* CopyWord(Word source){
  return CreateWord(source.file_id,source.doc_id,source.size,source.start,
                    source.wordptr);
}

char getLetter(Word word, int letter_pos){
  return word.wordptr[letter_pos];
}

void PrintWord(Word word){
  for(int i=0; i<(word.size); i++){
    printf("%c", getLetter(word,i));
  }
}
