#ifndef ERRORCODES_H
#define ERRORCODES_H

#include <stdio.h>

#include <stdlib.h>
typedef enum ERRORCODE { OK=0,
                  BAD_ARGUMENTS=1,
                  CANT_OPEN_FILE,
                  ALLOCATION_FAIL,
                  UNKNOWN_CMDARGUMENT,
                  UNSPECIFIED_i,
                  w_LESS_THAN_ONE
} ERRORCODE;

//this informs about the error that terminated the prog abruptly
ERRORCODE ERROR;

//used after allocating/reallocating memory to check weather it was succesfull
#define NULL_Check(ptr) if(ptr == NULL) {fprintf(stderr, "Malloc/Realloc failed %s:%d\n", __FILE__, __LINE__); exit(ALLOCATION_FAIL);}

#endif
