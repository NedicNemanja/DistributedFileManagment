#ifndef POST_H
#define POST_H
#include "Word.h"

//one word appears in many documents, 1 post per document
typedef struct Post{
  int file_id;           //the id of the file where the word was found
  unsigned int doc_id;  //the line of the file where the word was found
  unsigned int recurrence;  //num of times the word recurres in doc
  int* word_posArray; //unique positions of where the word starts in this doc/line
  struct Post* next;   //next post of the list
}Post;

Post* CreatePost(Word word);

void FreePost(Post* post);

//When a word is found, we mark where in the document we found it
void Post_AddWordPosition(Post* post, int word_start);
#endif
