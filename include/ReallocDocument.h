#ifndef REALLOCDOCUMENT_H
#define REALLOCDOCUMENT_H

#define GOLDEN_RATIO 1.61803398875

/*Make the document size GOLDEN_RATIO times bigger.*/
char* ReallocDocument(char* doc, int* doc_size);

/*Trim the docmuent size down to new_size.*/
char* ResizeDocument(char* doc, int new_size);

#endif
