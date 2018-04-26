#ifndef QUERRY_H
#define QUERRY_H

typedef struct Querry{
  char** q;
  unsigned int size;
}Querry;

Querry* CreateQuerry(char* msg);

void FreeQuerry(Querry* querry);

#endif
