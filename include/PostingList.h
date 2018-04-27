#ifndef POSTINGLIST_H
#define POSTINGLIST_H

#include "Post.h"
#include "Word.h"
#include "List.h"

/*head of the posting list
note: posts are in reverse order,
      new posts are appended to the start of the list*/
typedef struct PostingList{
  unsigned int term_frequency;  //num of times this word is found is the file
  unsigned int doc_frequency;   /*num of documents that contain this word,
                                  and subsequently the number of posts*/
  Post* post;   //ptr to the 1st post
  Word* word;   //this one we need in case /df is called
}PostingList;

//This is our global List of PostingLists
ListNode PLIST;

//allocate a new posting list and initialize it
PostingList* CreatePostingList(Word word);

//delete everything dynamically allcoated for this posting list
void FreePostingList(PostingList* pl);

//free all the posts of a posting list recursively
void FreePosts(Post* post);

//get the post at index from this posting list
Post* getPost(PostingList* pl, int index);

/*This is called when a word that already exists in the Trie is found.
If a post for this document already exists, then: recurrence+1
Else create a new post for this document and add it to the start of the list.
This means that we only need to check the 1st post of the list in order to
learn if it already exists or not.*/
void AddPost(PostingList* plist, Word word);

void PrintPostingList(PostingList*);
void PrintAllPostingLists(ListNode*);
void PrintRecurrence(PostingList pl, int doc_id);

/*Given a PostingList group all its posts by file_id in a 2d array of Post*.*/
void GroupByFile(Post*** PostsByFile, int* PostsInFile, PostingList* pl);

#endif
