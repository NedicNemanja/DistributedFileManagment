#ifndef TRIE_H
#define TRIE_H
#include "AVL_tree.h"
#include "PostingList.h"

/*A TrieNode contains an AVLnode.*/
typedef struct TrieNode{
  AVLnode avlnode;
  PostingList* plist;
  struct TrieNode* sub_tree;
}TrieNode;

//This is our global Trie
TrieNode* TrieRoot;

/*Insert a word in the Trie or if the word already exists just +1 to its plist.
Every level of Trie is implemented as an AVLnode, so in order to know when a new
node has been created and fix the balance we keep a parent_call_flag for each
avl tree.*/
TrieNode* TrieInsert(TrieNode* node, Word* word,int key_size,int* parent_call_flag);

PostingList* SearchTrie(char* word,TrieNode* node, int key_size);

TrieNode* CreateTrieNode(char key);

/*Recursively free the trie by first freeing the sub_tree,
then left and then right nodes of each node. Then free the node itself.*/
void FreeTrie(TrieNode* node);

/*Frees the node and its posting list.*/
void FreeTrieNode(TrieNode* node);

/*Get the key from the node->avlnode*/
char getKey(TrieNode* node);

/*print the whole avl tree of a trie level*/
void PrintTrieLevel(TrieNode* root);

#endif
