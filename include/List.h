#ifndef LIST_H
#define LIST_H

//generic List
typedef struct ListNode{
  void* data;
  struct ListNode* next;
}ListNode;

/*********************members*********************************************/

ListNode* CreateNode(void* data);

/****************external use**********************************************/

//insert a new node in the list
void ListInsert(ListNode* new_node, void* data);

//Delete the list and its data recursively
void FreeList(ListNode* node);

#endif
