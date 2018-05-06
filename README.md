A distributed file managment system implemented in C for the class of Systems Programming
as an exercise on inter-process communication using signals and (named) pipes.

The idea is to have a central process(jobExecutor) that interacts with the user and use its
children processes (workers) to manage a set of directiores.
i.e. The jobExecutor send out questions to all its workers and the workers answer based on the
files they are managing (or they don't in which case i handle these failiures).

# Compile and run
A Makefile is provided. (-g is default).

Run the program with 

                  ./jobExecutor -i [inputfile.txt] -w [num of workers]
inputfile.txt contains the paths (absolute or reltive) to the set of directiores that the workers will manage.

# Console options:

There are 4 basic commands which you can use:
  ```
  /search [Query] -d deadline
  ```
  To search the file with a query and a deadline in seconds.
  In case deadline is up the jobExecutor will stop receiving answers and will print informing if any workers failed to answer in time.
  Also the workers are informed(using SIGUSR1) by the parent that the deadline is up and that they shouldn't go on answering.
  ```
  /maxcount keyword
  ```
  To find the file that contains the keyword the most.
  ```
  /mincount keyword
  ```
  To find the file that contains the keyword the least.
  ```
  /wc
  ```
  To print that total number of lines,words & bytes(printable chars not counting newline) in all the directories managed.
  ```
  /exit
  ```
  To exit the console and terminate the program freeing allocated memory and making sure no zombie processes are leftover
  or any unlinked pipes.

Also, for each worker log files are kept and some scripts are provided in order to check the results efficiently.
Logs are formated as:
```
Time of query arrival : Query type : string : pathname1 : pathname2 : pathname3 : ... : pathnameN
```
