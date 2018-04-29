#include "DocumentMap.h"
#include "ErrorCodes.h"
#include <stdlib.h>
#include <string.h>

DocumentMAP* CreateMap(){
  DocumentMAP* map = malloc(sizeof(DocumentMAP));
  NULL_Check(map);
  map->size = 0;
  map->map = NULL;
  map->doc_lenght = NULL;
  return map;
}

void FreeMap(DocumentMAP* DMAP){
  //free all the documents
  for(int i=0; i<DMAP->size; i++){
    if(DMAP->map[i] != NULL)
      free(DMAP->map[i]);
    DMAP->map[i] = NULL;
  }
  //free the array for keeping track of document lenghts
  if(DMAP->doc_lenght != NULL){
    if(DMAP->doc_lenght != NULL)
      free(DMAP->doc_lenght);
    DMAP->doc_lenght = NULL;
  }
  //Free the map array where we kept the documents
  if(DMAP->map != NULL){
    free(DMAP->map);
    DMAP->map = NULL;
  }
}

/*Make a new slot in the map for a new document. Allocate space for it.*/
void NewDocument(DocumentMAP* DMAP,int doc_size){
  int doc_index = DMAP->size;
  //resize the map
  DMAP->size++;
  DMAP->map = realloc(DMAP->map,sizeof(char*)*DMAP->size);
  NULL_Check(DMAP->map);
  //resize the doc_lenght array as well
  DMAP->doc_lenght = realloc(DMAP->doc_lenght,sizeof(int)*DMAP->size);
  DMAP->doc_lenght[doc_index] = 0;
  //allocate space for this document in the map
  DMAP->map[doc_index] = malloc(sizeof(char)*(doc_size +1)); //+1 for the \0
  NULL_Check(DMAP->map[doc_index]);
}

void DeleteDocument(DocumentMAP* Dmap){
  //free document
  free(Dmap->map[Dmap->size-1]);
  //resize map
  Dmap->size--;
  Dmap->map = realloc(Dmap->map,sizeof(char*)*Dmap->size);
  NULL_Check(Dmap->map);
}


void PrintMAP(DocumentMAP DMAP){
  for(int i=0; i<DMAP.size; i++){
    if(DMAP.map[i] == NULL && i != DMAP.size-1)
      printf("(size:0)%d", i);
    if(DMAP.map[i] != NULL)
      printf("(size:%d)%d %s", (int)strlen(DMAP.map[i]),i,DMAP.map[i]);
    printf("\n");
  }
}

unsigned int GetNumLines(DocumentMAP** DocMaps, int numFiles){
  unsigned int total_lines=0;
  for(int i=0; i<numFiles; i++){
    total_lines += DocMaps[i]->size;
  }
  return total_lines;
}
