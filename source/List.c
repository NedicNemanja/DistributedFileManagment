#include <stdlib.h>
#include "List.h"
#include "ErrorCodes.h"


ListNode* CreateNode(void* data){
  ListNode* new_node = malloc(sizeof(ListNode));
  NULL_Check(new_node);

  new_node->data = data;
}

//insert a new node in the list
void ListInsert(ListNode* head, void* new_data){
  ListNode* new_node = CreateNode(new_data);
  //put it at the start of the list
  ListNode* temp_next = head->next;
  head->next = new_node;
  new_node->next = temp_next;
}

void FreeList(ListNode* node){
  if(node == NULL)
    return;
  //free next
  FreeList(node->next);
  node->next = NULL;
  //free urself
  free(node);
}
