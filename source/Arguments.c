#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Arguments.h"
#include "ErrorCodes.h"

void ReadArguments(int argc, char* argv[]){
  int arg_index=1;
  int doc_flag=0, w_flag=0;

  //scan all arguements to find (-i,-k)
  while(arg_index < argc){
    //make sure there is an argument to check
    if(argv[arg_index] == NULL)
      break;

    if(!strcmp(argv[arg_index],"-i")){ //next arv_index is docfilename
      docfilename = argv[++arg_index];
      doc_flag = 1;
    }
    else if(!strcmp(argv[arg_index],"-w")){  //next arg_index is k
      numWorkers = atoi(argv[++arg_index]);
      if(numWorkers < 1){
        fprintf(stderr, "w cannot be less than 1.\n");
        ERROR = w_LESS_THAN_ONE;
        return;
      }
      w_flag = 1;
    }
    else{ //this argument should not be here
      fprintf(stderr, "Unknown argument: %s\n", argv[arg_index]);
      ERROR = UNKNOWN_CMDARGUMENT;
      return;
    }
    //next
    arg_index++;
  }

  //check that you got all the arguements
  if(doc_flag == 0){
    fprintf(stderr, "Specify a file to read documents from.\n"
                    "Example: ./minisearch -i docfile.txt\n");
    ERROR = UNSPECIFIED_i;
    return;
  }
  if(w_flag == 0){  //default: w=3
    numWorkers = 3;
    w_flag = 1;
  }
}
