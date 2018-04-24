#ifndef STRINGMANIPULATION_H
#define STRINGMANIPULATION_H


/*Skip whitespace and return the first non-whitespace char you find.*/
char SkipWhitespace(FILE* fp);

/*print n char of a doc string.*/
void PrintChars(char* doc, int* index, int n);

void PrintWhitespace(int n);

/*
/*Get the pointer of a word IN the document, that points at the first word
after offset-many characters*
char* getNextWord(char* doc, int* offset, int* word_size);

/*ignore and count every char in fstream until \n or EOF*
char GoToEndOfline(FILE* fp, int* DSize);
*/
#endif
