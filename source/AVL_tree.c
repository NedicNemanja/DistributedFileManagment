#include "AVL_tree.h"
#include <stdlib.h>


void InitAVLnode(AVLnode* node, char key){
  node->key = key;
  node->height = 1;
  node->left = NULL;
  node->right = NULL;
}

int Height(AVLnode node){
  return node.height;
}
