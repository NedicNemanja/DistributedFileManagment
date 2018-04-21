#ifndef PIPING_N
#define PIPING_N

#define PIPE_DIR "./pipes/"

/*Make 2 named pipes, one for the worker to read and one to write*/
int MakePipePair(int i);

/*Returns the conventional name of a workers pipe ex "to1"
CAUTION: user must free the pipename you got after this call.*/
char* PipeName(const char* str, int i);

/*number of digits in an int in base 10*/
int NumDigits(int i);

#endif
