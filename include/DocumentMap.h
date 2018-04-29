#ifndef DOCUMENTMAP_H
#define DOCUMENTMAP_H

//all the documents are kept here, indexing is the same as in the input file
typedef struct DocumentMAP{
  int size;     //number of documents in the map array
  char** map;  //flexible array for storing documents
  int* doc_lenght;  //array for keping track of num of words in each document
} DocumentMAP;

//Create empty map
DocumentMAP* CreateMap();

void FreeMap(DocumentMAP* DMAP);

/*Make a new slot in the map for a new document. Allocate space for it.*/
void NewDocument(DocumentMAP* DMAP,int doc_size);

//Free last document from map and make it smaller by 1
void DeleteDocument(DocumentMAP* DMAP);

void PrintMAP(DocumentMAP DMAP);

/*Return the total number of lines of all maps in DocMaps array*/
unsigned int GetNumLines(DocumentMAP** DocMaps, int numFiles);

#endif
