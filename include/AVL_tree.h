#ifndef AVL_TREE_H
#define AVL_TREE_H

//just a forward delcaration for the AVLnode-TrieNode circular dependency
struct TrieNode;

/*This is not a fully implemeted struct, its a part of larger
Trie. I just use this to organize each level of Trie in an AVL tree.*/
typedef struct AVLnode{
  char key;
  int height;
  struct TrieNode* left;
  struct TrieNode* right;
} AVLnode;

void InitAVLnode(AVLnode* node, char key);

int Height(AVLnode node);

#endif
