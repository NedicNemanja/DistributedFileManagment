#ifndef READPATHS_H
#define READPATHS_H


/*Reads the paths from docfilename and stores them in an array that is returned.
numPathsptr informs about the size of the array
If no paths a read then NULL is returned.
The array and its paths must be freed by the user program.*/
char** ReadPaths(char* docfilename, int* numPathsptr);

/*Free a char* array*/
void FreePaths(char** Paths,int size);

//*****Utility**********************************
void RemoveNewline(char** lineptr,ssize_t size);

#endif
