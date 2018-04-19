#ifndef ARGUMENTS_H
#define ARGUMENTS_H

char* docfilename;  //filename containg the paths
int numWorkers;     //num of workers, default is 3 if not specified


/*Read the command line arguments specified by -i,-w
and store them in the variables above^.*/
void ReadArguments(int argc, char* argv[]);

#endif
