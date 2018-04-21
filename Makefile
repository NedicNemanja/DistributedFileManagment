INCLUDE_DIR = ./include
SOURCE_DIR = ./source
OBJECT_DIR = ./object
PIPE_DIR = ./pipes

CC = gcc
CFLAGS=-g -I$(INCLUDE_DIR) -lm
OUT = jobExecutor

_DEPS = Arguments.h ErrorCodes.h Piping.h ReadPaths.h
#pattern matching from  _DEPS to include directory
DEPS = $(patsubst %,$(INCLUDE_DIR)/%,$(_DEPS))

_OBJ = Arguments.o JobExecutor.o Piping.o ReadPaths.o
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
	rm -f $(OBJECT_DIR)/*.o ./jobExecutor $(PIPE_DIR)/*
rmpipes:
	rm -f $(PIPE_DIR)/*
