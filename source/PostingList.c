#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "PostingList.h"
#include "ErrorCodes.h"

/*Here we keep all our posting lists and their words on a sorted manner,
for the /df option. This is a head node its data is always empty.*/
ListNode PLIST = {NULL,NULL};

PostingList* CreatePostingList(Word word){
  //allocate
  PostingList* new_plist = malloc(sizeof(PostingList));
  NULL_Check(new_plist);
  //initialize
  new_plist->term_frequency = 1;
  new_plist->doc_frequency = 1;
  new_plist->post = CreatePost(word);
  //the word-pointer will be freed, but we wish to save it, so lets keep a copy
  Word* copy_word = CopyWord(word);
  new_plist->word = copy_word;
  //insert to PLIST
  ListInsert(&PLIST,new_plist);
  return new_plist;
}

//delete everything dynamically allcoated for this posting list
void FreePostingList(PostingList* pl){
  if(pl == NULL)
    return;

  free(pl->word);
  pl->word = NULL;

  FreePosts(pl->post);
  pl->post = NULL;

  free(pl);
}

//free all the posts of a posting list recursively
void FreePosts(Post* post){
  if(post == NULL)
    return;

  FreePosts(post->next);
  post->next = NULL;

  FreePost(post);
}

//get the post at index from this posting list
Post* getPost(PostingList* pl, int index){
    //might wanna check? if(index<0 || index>pl.doc_frequency-1)
  Post* post = pl->post;
  for(int i=1; i<=index; i++){
    post = post->next;
  }
  return post;
}


/*This is called when a word that already exists in the Trie is found.
If a post for this document already exists, then: recurrence+1
Else create a new post for this document and add it to the start of the list.
This means that we only need to check the 1st post of the list in order to
learn if it already exists or not.
Also we need to inform the post about the position of the word in this doc.*/
void AddPost(PostingList* plist, Word word){
  (plist->term_frequency)++;
  //if post already exists in this document then we dont need a new post for it
  if(plist->post->doc_id==word.doc_id && word.file_id==plist->post->file_id){
    (plist->post->recurrence)++;
    //remember where you found this word in the document
    Post_AddWordPosition(plist->post,word.start);
  }
  //if not (its the first time for this document) create a new one
  else{
    (plist->doc_frequency)++;
    //append the new post to the start of the list
    Post* temp_post = plist->post;
    plist->post = CreatePost(word);
    plist->post->next = temp_post;
  }
}


void PrintAllPostingLists(ListNode* list){
  while(list != NULL){
    if(list->data != NULL){
      PostingList* plist = list->data;
      PrintPostingList(plist);
    }
    list = list->next;
  }
}

void PrintPostingList(PostingList* plist){
  if(plist != NULL){
    PrintWord(*(plist->word));
    printf(" %d\n", plist->doc_frequency);
  }
}

//find the post with doc_id inside a posting list, and print its reccurence
void PrintRecurrence(PostingList pl, int doc_id){
  Post* post = pl.post;
  int print_flag = 0;

  while(post != NULL){
    if(post->doc_id == doc_id){ //found document
      printf("%d ", post->recurrence);
      PrintWord(*pl.word);
      printf("\n");
      print_flag = 1;
    }
    post = post->next;
  }
  if(print_flag == 0){  //didnt find document at all
    printf("[");
    PrintWord(*pl.word);
    printf("] does not appear in %d document.\n", doc_id);
  }
}


/*Given a PostingList group all its posts by file_id in a 2d array of Post*.*/
void GroupByFile(Post*** PostsByFile, int* PostsInFile, PostingList* pl){
  //for every post of this list find its file_id
  for(int j=0; j<pl->doc_frequency; j++){
    Post* post = getPost(pl,j);
    //make room for one more Post* in this file
    PostsInFile[post->file_id]++;
    PostsByFile[post->file_id] = realloc(PostsByFile[post->file_id],
                                    sizeof(Post*)*(PostsInFile[post->file_id]));
    //and insert it in the back fo the array
    PostsByFile[post->file_id][PostsInFile[post->file_id]-1] = post;
  }
}
