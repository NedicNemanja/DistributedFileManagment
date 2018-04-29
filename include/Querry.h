#ifndef QUERRY_H
#define QUERRY_H

typedef struct Querry{
  char** q;
  unsigned int size;
}Querry;

//Make a querry from a msg
Querry* CreateQuerry(char* msg);

/*Read from stdin until you find "-d" and create a querry.
On fail return NULL*/
Querry* CreateQuerryStdin();

void FreeQuerry(Querry* querry);

//return all the words as a string
char* QuerryToStr(Querry* querry,const char* delimeter);

#endif
