#include <stdio.h>
#include <stdlib.h>
#include "ReallocDocument.h"

/*Make the document size GOLDEN_RATIO times bigger.
+1 is for \0.*/
char* ReallocDocument(char* doc, int* doc_size){
  (*doc_size) *= GOLDEN_RATIO;
  char* ptr = realloc( doc, sizeof(char)*( *doc_size +1) );
  return ptr;
}

char* ResizeDocument(char* doc, int new_size){
  return realloc( doc,sizeof(char)*(new_size+1) );
}
