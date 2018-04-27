#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "StringManipulation.h"
#include "ErrorCodes.h"
#include "ReallocDocument.h"
#include <sys/ioctl.h>
#include <math.h>

/*Skip whitespace and return the first non-whitespace char you find.*/
char SkipWhitespace(FILE* fp){
  char c;
  do{
      c = fgetc(fp);
  }while(c == ' ' || c == '\t');
  return c;
}

/*print n char of a doc string,
return 1 if the string ends before chars are printed.*/
void PrintChars(char* doc, int* index, int n){
  for(int i=0; i<n; i++){
    if(doc[(*index)] == '\0'){
      //if you reach the end of document
      //fill it with whitespace
      for(int j=i; j<n; j++)
        printf(" ");
      return;
    }
    printf("%c", doc[(*index)]);
    (*index)++;
  }
}

void PrintWhitespace(int n){
  for(int i=0; i<n; i++){
    printf(" ");
  }
}

//return the first word from msg
char* getInstruction(char* msg){
  char* instruction = NULL;
  int instr_size=0;
  int msg_index = 0;
  while(msg[msg_index] != ' '){
    instr_size++;
    instruction = realloc(instruction,sizeof(char)*(instr_size+1));
    NULL_Check(instruction);
    instruction[instr_size-1] = msg[msg_index];
    msg_index++;
  }
  instruction[instr_size] = '\0';
  return instruction;
}

int NumDigits(int i){
  if(i==0)
  return 1;
  /*do i+1 because in the case of i=10 log10(10)=1 and we need 2*/
  return (int)ceil(log10(i+1));
}



/*Find where the next word in the document starts.
Keep track of the offset inside the document and count the size of the word.
If no more words are found return NULL.*/
char* getNextWord(char* doc, int* offset, int* word_size){
  *word_size = 0;
  //exception for end of document
  if(doc[*offset] == '\0')
    return NULL;

  //find where the word starts
  while((doc[*offset] == ' ' || doc[*offset] == '\t') && doc[*offset]!='\0')
    (*offset)++;
  int start = *offset;
  //exception for end of document
  if(doc[*offset] == '\0')
    return NULL;

  //read the word
  while(doc[*offset] != ' ' && doc[*offset] != '\t' && doc[*offset] != '\0'){
    (*offset)++;
    (*word_size)++;
  }
  return doc+start;
}

/*ignore and count every char in fstream until \n or EOF*/
char GoToEndOfline(FILE* fp, int* DSize){
  char c = fgetc(fp);	//ignore the space between index and 1st word
  while(c != '\n' && c!=EOF){
    c=fgetc(fp);
    if(c!='\n' && c!=EOF)
      (*DSize)++;
  }
  return c;
}


/*diff version unused
char* getnextWord(char* doc, int* offset){
  //exception for end of document
  if(doc[*offset] == '\0')
    return NULL;

  //if this is the first word, we already know where it starts
  if(*offset == 0){
    while(doc[*offset] != ' ' && doc[*offset] != '\t' && doc[*offset] != '\0'){
        printf("%d%c\n", *offset,doc[*offset]);
      (*offset)++;  //just increase offset until we pass this word
    }
    return doc;
  }

  //else skip until the whitespace
  while(doc[*offset] != ' ' && doc[*offset] != '\t' && doc[*offset] != '\0')
    (*offset)++;
  //exception for last word of the document
  if(doc[*offset-1] == '\0')
    return NULL;

  //skip the whitespace
  while(doc[*offset] == ' ' || doc[*offset] == '\t')
    (*offset)++;
  //exception for the whitespace at the end of the document
  if(doc[*offset-1] == '\0')
    return NULL;

  return doc+(*offset);
}
*/
