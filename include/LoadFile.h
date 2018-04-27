#ifndef LOADFILE_H
#define LOADFILE_H

#include "DocumentMap.h"

/******************global variables*******************************************/

/*total count of all doc sizes of all the files*/
unsigned long long int TOTAL_SIZE;

unsigned long long int MEAN_DOC_SIZE;/*mean size of a document in characters
                                          without \0. Consecutive whitespace
                                          counts as 1 character.*/

unsigned long long int WORDS_IN_FILES; /*total num of words in all files.*/

/*Takes a open fp and maps its contents to a DocumentMAP + loads its words to
the Trie.*/
DocumentMAP* LoadFile(FILE* fp,int fild_id);

/*Load all files in FIles to memory. Skip the files that you cant open.
Return an array of the DocumentMAPs.*/
DocumentMAP** LoadFiles(char** FilePaths,int numFiles);

void FreeDocMaps(DocumentMAP** DocMaps,int numFiles);

/******************************************************************************
****************************Local members - NOT used externally****************
******************************************************************************/

void UpdateMEAN_DOC_SIZE(int new_doc_size,int dmap_size);
void PrintMAP();
void AddWord(FILE* fp, char** doc_ptr, int* doc_size, int* char_index, char* c);
void FreeMap();

#endif
