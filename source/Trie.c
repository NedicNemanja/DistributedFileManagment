#include <stdlib.h>
#include <string.h>
#include "Trie.h"
#include "ErrorCodes.h"
#include "Word.h"
#include "Querry.h"

//This is our Trie
TrieNode* TrieRoot = NULL;

/************************TrieNode memebers*************************************/

TrieNode* CreateTrieNode(char key){
  TrieNode* node = malloc(sizeof(TrieNode));
  NULL_Check(node);

  InitAVLnode(&(node->avlnode),key);

  node->plist = NULL;
  node->sub_tree = NULL;
}

char getKey(TrieNode* node){
  return node->avlnode.key;
}

/*********************Trie INSERT**********************************************/
//utility macros
#define MAX(a,b) ( (a)>(b) ? (a):(b) )

#define LnodeHeight(n)\
  ((n)->avlnode.left ==NULL ? 0:((n)->avlnode.left->avlnode.height))
#define RnodeHeight(n)\
  ((n)->avlnode.right==NULL ? 0:((n)->avlnode.right->avlnode.height))

//Typical right rotation for an AVL node
TrieNode* RightRotation(TrieNode* node){
  TrieNode* leftnode = node->avlnode.left;
  TrieNode* leftrightnode = leftnode->avlnode.right;

  //rotate
  leftnode->avlnode.right = node;//original node comes down as child of leftnode
  node->avlnode.left = leftrightnode; //lrnode becomes child of original node
  //reset height
  node->avlnode.height = MAX(LnodeHeight(node),RnodeHeight(node))+1;
  leftnode->avlnode.height = MAX(LnodeHeight(leftnode),RnodeHeight(leftnode))+1;

  //left node is now a root
  return leftnode;
}

//Typical left rotation for an AVL node
TrieNode* LeftRotation(TrieNode* node){
  TrieNode* rightnode = node->avlnode.right;
  TrieNode* rightleftnode = rightnode->avlnode.left;

  //rotate
  rightnode->avlnode.left =node;//original node comes down as child of rightnode
  node->avlnode.right = rightleftnode;  //rlnode becomes child of original node
  //reset height
  node->avlnode.height = MAX(LnodeHeight(node),RnodeHeight(node))+1;
  rightnode->avlnode.height=MAX(LnodeHeight(rightnode),RnodeHeight(rightnode))+1;

  //right node is now a root
  return rightnode;
}


/*Given a node insert in its trie for a set of keys(word) one by one.***********

If you find the key already exists key then do nothing and drop into its
sub_tree in order to search for the next key.

If you enounter NULL before you find the node with the key,
then create a new node with the key and search its sub_tree for the next key.
(when a new node is created balance of the avl treee must be restored)

When there are no more keys (key_size==0) create a posting list for this word.

The return value is always the node we called the function for, except
when we call for NULL and a new node is created we return the new node or when,
in order to restore balance, we swapped some nodes.

Note: im passing the word by reference(pointer), not by value, in order to save
stack memory as this is a recursive function. In no case is the original word
being altered in this function.
*/

TrieNode* TrieInsert(TrieNode* node, Word* word,int key_size, int* parent_call_flag){
  /*if it happens that we create a node in some branch of this tree,
  we need to inform the parents about this action so that a balance check is done.*/
  int local_flag = 0;
  char key = getLetter(*word,key_size-1);
  /***********Search the Trie for the node with key****************************/
  //if you expected a node here but didnt find it, then create it
  if(node == NULL){
      node = CreateTrieNode(key);
      *parent_call_flag = 1;
  }
  //keep searching
  if(key < node->avlnode.key){
    node->avlnode.left = TrieInsert(node->avlnode.left,word,key_size,&local_flag);
  }
  else if (key > node->avlnode.key){
    node->avlnode.right = TrieInsert(node->avlnode.right,word,key_size,&local_flag);
  }
  else{ //if you find the node with the key(base case)
    if(key_size == word->size){  //and you reach the end of word, stop searching
      if(node->plist == NULL)
        node->plist = CreatePostingList(*word);
      else
        AddPost(node->plist,*word);
    }
    else{//otherwise drop into the sub tree to keep searching for the other keys
      int ignore; //since this is a diff. tree ignore the balance connection
      node->sub_tree = TrieInsert(node->sub_tree, word, key_size+1, &ignore);
    }
  }

  if(local_flag == 1){
  /********Update height after inserting***************************************/
  node->avlnode.height = MAX(LnodeHeight(node),RnodeHeight(node)) +1;

  /*********Check Balance******************************************************/
    *parent_call_flag = 1;//all the parents must be informed about balance change

    int balance = LnodeHeight(node)-RnodeHeight(node);

    //unbalanced Left
    if(balance > 1){
      //Left-Left
      if(key < getKey(node->avlnode.left)){
        node = RightRotation(node);
      }
      //Left-Right
      if(key > getKey(node->avlnode.left)){
        node->avlnode.left = LeftRotation(node->avlnode.left);
        node = RightRotation(node);
      }
    }
    //unbalanced Right
    else if(balance < -1){
      //Right-Left
      if(key < getKey(node->avlnode.right)){
        node->avlnode.right = RightRotation(node->avlnode.right);
        node = LeftRotation(node);
      }
      //Right-Right
      if(key > getKey(node->avlnode.right)){
        node = LeftRotation(node);
      }
    }
  }

  //inform the parent about its new child or return the original ptr
  return node;
}

/*********************TRIE SEARCH**********************************************/
//for 1 word
PostingList* SearchTrie(char* word,TrieNode* node, int key_size){
  char key = word[key_size-1];
  if(node == NULL){  //base case1: if the word existed a node should be here
    return NULL;
  }

  /*search this tree for the key, when you find it descend into its sub_tree*/
  if(key < node->avlnode.key){
    //the key is left of this node
    return SearchTrie(word,node->avlnode.left,key_size);
  }
  else if(key > node->avlnode.key){
    //the key is right of this node
    return SearchTrie(word,node->avlnode.right,key_size);
  }
  else{ //found the key
    //if this is the last node of this word
    if(key_size == strlen(word)){
      return node->plist;
    }
    //otherwise descend into sub_tree to search for the next key of the word
    return SearchTrie(word,node->sub_tree,key_size+1);
  }
}

//for a whole querry find the results(posting lists) and return an array of them
PostingList** SearchTrieQuerry(Querry* querry, int* numResults){
  //find all the posting lists
  PostingList** Results = NULL;
  for(int i=0; i<querry->size; i++){
    //if alarm deadline break
    PostingList* res = SearchTrie(querry->q[i],TrieRoot,1);
    if(res != NULL){  //if found
      //resize Results array
      (*numResults)++;
      Results = realloc(Results,sizeof(PostingList*)*(*numResults));
      NULL_Check(Results);
      //insert
      Results[(*numResults)-1] = res;
    }
  }

  return Results;
}

/********************TRIE FREE*************************************************/

void FreeTrie(TrieNode* node){
  if(node == NULL)  //base case
    return;
  //first free the sub_tree
  FreeTrie(node->sub_tree);
  node->sub_tree = NULL;
  //free all the nodes left of this one
  FreeTrie(node->avlnode.left);
  node->avlnode.left = NULL;
  //and all the nodes right of this one
  FreeTrie(node->avlnode.right);
  node->avlnode.right = NULL;
  //https://www.youtube.com/watch?v=D_AYbGWHgSs
  FreeTrieNode(node);
}

void FreeTrieNode(TrieNode* node){
  FreePostingList(node->plist);
  node->plist = NULL;

  free(node);
}

void PrintTrieLevel(TrieNode* node){
  if(node == NULL)
    return;
  printf(" %c", node->avlnode.key);
  PrintTrieLevel(node->avlnode.left);
  PrintTrieLevel(node->avlnode.right);

}
