#include <stdlib.h>
#include "Post.h"
#include "ErrorCodes.h"

Post* CreatePost(Word word){
  Post* post = malloc(sizeof(Post));
  NULL_Check(post);
  //initalize
  post->path = word.path;
  post->doc_id = word.doc_id;
  post->recurrence = 1;
  //mark where you found the word in doc
  post->word_posArray = malloc(sizeof(int));
  post->word_posArray[0] = word.start;

  post->next = NULL;
  return post;
}

void FreePost(Post* post){
  if(post == NULL)
    return;

  free(post->word_posArray);
  post->word_posArray = NULL;

  free(post);
}

//When a word is found, we mark where in the document we found it
void Post_AddWordPosition(Post* post, int word_start){
  int array_size = post->recurrence;
  post->word_posArray = realloc(post->word_posArray, sizeof(int)*array_size);
  NULL_Check(post->word_posArray);
  post->word_posArray[array_size-1] = word_start;
}
