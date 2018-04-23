#ifndef READPATHS_H
#define READPATHS_H

#include <sys/types.h>

/*Reads the paths from docfilename and stores them in an array that is returned.
numPathsptr informs about the size of the array
If no paths a read then NULL is returned.
The array and its paths must be freed by the user program.*/
char** ReadPaths(char* docfilename, int* numPathsptr);

/*Free a char* array*/
void FreePaths(char** Paths,int size);

/*Infrom the Children processes about the paths of directories they will work
on thru the named pipes.*/
void DistributePaths(pid_t* Children, char** Paths, int numPaths,
                                                int* OpenToPipes);

/********************Utility**********************************/
void RemoveNewline(char** lineptr,ssize_t size);

/*Concatenate load_size many strings from the Paths array in a single string
that is returned. Start from the Paths[Paths_index].
This function actually decides how the paths are distributed by packing them
into loads, so if you want to distribute in a different manner edit this one.*/
char* MakeLoad(char** Paths, int numPaths,int* Paths_indexptr, int load_size);

#endif
