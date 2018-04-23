#ifndef PIPING_N
#define PIPING_N

#include <sys/types.h>


#define PIPE_DIR "./pipes/"

/*Before sending a message thru the pipe we send the PipeHead.
If the PipeHeaders left most bit(lmb) is 1 then the message is a part of a
sequence of messages that all have a 1 as the lmb of the PipeHeaders,
the last PipeHeader of the sequence has a lmb 0 thats how the sequence breaks.
(a message sequence means that it is to be interpreted as one message,
but could not fit the pipe buffer whole)*/
typedef int PipeHead;

#define MAX_MSG_SIZE 1000

/*This is a flag so the process knows how many messages are
available for reading from the pipe. At the start of every porcess this
flag is set to 0.*/
int READ_FLAG;

/*Make 2 named pipes, one for the worker to read and one to write*/
int MakePipePair(int i);

/*Returns the conventional name of a workers pipe
example: PipeName("to",1) returns "./PipeDirectory/to1"
CAUTION: user must free the pipename you got after this call.*/
char* PipeName(const char* str, int i);

/*number of digits in an int in base 10*/
int NumDigits(int i);

/*Send a msg to a file descriptor using write.
First send a  message header that contains info about the size of the message
and then the actual message.
Messages with size > MAX_MSG_SIZE are broken up into a sequence of messsages
(Read PipeHead)
At the end send SIGUSR1 signal to the receiver so he knows he is getting a msg.
CAUTION:when a pipe is full the write(2) will block
        until sufficient data is read from the named pipe,
        thats why O_NONBLOCK must be disabled.
*/
void Send(pid_t receiver, int fd, char* msg);

char* Receive(int fd);

#endif
