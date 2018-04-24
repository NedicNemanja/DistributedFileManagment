#ifndef WORD_H
#define WORD_H

typedef struct Word{
  char* path;
  int doc_id;
  int size;
  int start; //where the word starts in the document
  char* wordptr;  //ptr to the word start in the document
}Word;

Word* CreateWord(char* path,int doc_id,int size,int start,char* wordptr);
Word* CopyWord(Word source);

char getLetter(Word word, int letter_pos);
void PrintWord(Word word);

#endif
