#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Arguments.h"
#include "ErrorCodes.h"
#include "StringManipulation.h"
#include "ReallocDocument.h"
#include "Trie.h"
#include "LoadFile.h"

/*total count of all doc sizes of all the files*/
unsigned long long int TOTAL_SIZE = 0;

unsigned long long int MEAN_DOC_SIZE = 5;/*mean size of a document in characters
                                          without \0. Consecutive whitespace
                                          counts as 1 character.*/

unsigned long long int WORDS_IN_FILES = 0; /*total num of words in all files.*/




/************************Reading file to create Map and Trie*******************/

DocumentMAP* LoadFile(FILE* fp,char* path){
  DocumentMAP* Dmap = CreateMap();
  char c;
  do{ //for every line
    //add a new document to the Dmap
    int doc_size = MEAN_DOC_SIZE;
    NewDocument(Dmap,doc_size);
    int char_index = -1;

    //read the document word by word and store every word in the map and trie
    do{
      //skip until the word starts
      c = SkipWhitespace(fp);
      if(c != '\n' && c != EOF){  //if you found a word (doc not empty)
        char_index++;
        int word_start = char_index;
        //append the word to the document (increase doc_size if needed)
        AddWord(fp,&(Dmap->map[Dmap->size-1]),&doc_size,&char_index,&c);
        Dmap->doc_lenght[Dmap->size-1]++;
        WORDS_IN_FILES++;
        //initialize the word you just got
        Word* word = CreateWord(path,                           //word->path
                                Dmap->size-1,                   //word->doc_id
                                char_index-word_start,          //word->size
                                word_start,                     //word->start
                                Dmap->map[Dmap->size-1]+word_start);//word->wptr
        //insert the word to the Trie
        int ignore;
        TrieRoot = TrieInsert(TrieRoot, word, 1, &ignore);
        //add a space after the word in the document
        //(the last space is overridden by \0 anyway)
        Dmap->map[Dmap->size-1][char_index] = ' ';
        free(word);
      }
    }while(c != '\n' && c != EOF);

    /****************document added***cleanup*********************************/
    //most likely there will be more space allocated than needed for a document
    if(char_index == -1){
      //empty documents need no space and are marked as NULL in the map
      free(Dmap->map[Dmap->size-1]);
      Dmap->map[Dmap->size-1] = NULL;
      UpdateMEAN_DOC_SIZE(0,Dmap->size);
    }
    else{
      Dmap->map[Dmap->size-1][char_index] = '\0'; //terminate the document
      if(char_index < doc_size){  //check if the size is precise, if not resize
        Dmap->map[Dmap->size-1] = ResizeDocument(Dmap->map[Dmap->size-1],char_index);
        NULL_Check(Dmap->map[Dmap->size-1]);
        UpdateMEAN_DOC_SIZE(char_index,Dmap->size);
      }
    }
  }while(c != EOF);

  return Dmap;
}

DocumentMAP** LoadFiles(char** FilePaths,int numFiles){
  DocumentMAP** DocMaps = malloc(sizeof(DocumentMAP*)*numFiles);
  NULL_Check(DocMaps);
  for(int i=0; i<numFiles; i++){
    //open file for reading
    FILE* fp = fopen(FilePaths[i],"r");
    //if you cant open skip it
    if(!fp){
      DocMaps[i] = NULL;
      fprintf(stderr,"Can't open file %s", FilePaths[i]);
      continue;
    }
    //load to map and trie
    DocMaps[i] = LoadFile(fp,FilePaths[i]);
    fclose(fp);
  }
  return DocMaps;
}

void FreeDocMaps(DocumentMAP** DocMaps,int numFiles){
  for(int i=0; i<numFiles; i++){
    FreeMap(DocMaps[i]);
    free(DocMaps[i]);
    DocMaps[i] = NULL;
  }
  free(DocMaps);
}

/*******************************************************************************
**************************Utility FUnctions for MapAndTrie**********************
********************************************************************************/
void UpdateMEAN_DOC_SIZE(int new_doc_size,int Dmap_size){
  TOTAL_SIZE += new_doc_size;
  MEAN_DOC_SIZE = ceil((double)TOTAL_SIZE/Dmap_size);
}

/*Read a stream of char from fp, and append them to the doc string until you find
whitespace,newline or EOF.
That last argument is valuable, its the first char we append.
While appending characters to the doc string,you might get out of its boundaries.
ReallocDocument() is used to resize our document and update the doc pointer in
the DMAP.*/
void AddWord(FILE* fp, char** doc_ptr, int* doc_size, int* char_index, char* c){
  char* doc = *doc_ptr;
  //while the word has not ended
  while(*c != ' ' && *c != '\t' && *c != '\n' && *c != EOF){
    //make document bigger if needed
    if(*char_index > *doc_size-2){
      *doc_ptr = ReallocDocument(doc,doc_size);
      NULL_Check(*doc_ptr);
      doc = *doc_ptr;
      NULL_Check(doc_ptr);
    }
    doc[*char_index] = *c;
    (*char_index)++;
    *c = fgetc(fp);
  }
}
