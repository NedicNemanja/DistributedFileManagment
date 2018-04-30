#ifndef WORD_H
#define WORD_H

typedef struct Word{
  int file_id;
  int doc_id;
  int size;
  int start; //where the word starts in the document
  char* wordptr;  //ptr to the word start in the document
}Word;

Word* CreateWord(int file_id,int doc_id,int size,int start,char* wordptr);
Word* CopyWord(Word source);

char getLetter(Word word, int letter_pos);
void PrintWord(Word word);

//allocate memory and return the word,user must free said memory
char* WordGet(Word* word);
#endif
