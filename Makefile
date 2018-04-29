INCLUDE_DIR = ./include
SOURCE_DIR = ./source
OBJECT_DIR = ./object
PIPE_DIR = ./pipes

CC = gcc
CFLAGS=-g -I$(INCLUDE_DIR) -lm
OUT = jobExecutor

_DEPS = Arguments.h AVL_tree.h Console.h DocumentMap.h ErrorCodes.h List.h LoadFile.h Log.h Piping.h Post.h PostingList.h Querry.h ReadPaths.h ReallocDocument.h StringManipulation.h Trie.h Word.h Worker.h
#pattern matching from  _DEPS to include directory
DEPS = $(patsubst %,$(INCLUDE_DIR)/%,$(_DEPS))

_OBJ = Arguments.o AVL_tree.o Console.o DocumentMap.o List.o LoadFile.o Log.o Piping.o Post.o PostingList.o Querry.o ReadPaths.o ReallocDocument.o StringManipulation.o Trie.o Word.o Worker.o JobExecutor.o
#same pattern matching principe
OBJ = $(patsubst %,$(OBJECT_DIR)/%,$(_OBJ))

.PHONY: clean rmpipes

############goals#######################

#general rule for all object files
$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

#default goal
$(OUT): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

count:
	wc $(SOURCE_DIR)/*.c $(DEPS)

clean:
	rm -f $(OBJECT_DIR)/*.o ./jobExecutor $(PIPE_DIR)/* ./log/*
rmpipes:
	rm -f $(PIPE_DIR)/*
