#my general makefile template
CXX = gcc
CFLAGS = -Wall #-fpic

#Application-related macros
SRC = main.c voie.c aiguillage.c train.c list.c structures.c
OBJ = $(SRC:.c=.o)
HEAD= voie.h aiguillage.h train.h list.h structures.h
TARGET = aiguillage.exe
ARGS = 

#Running the program
run: $(TARGET)
	@echo "\n Executing the executable " $(TARGET)
	./$(TARGET) $(ARGS)

build: $(TARGET)

#Generating the executable
$(TARGET): $(OBJ) $(HEAD)
	@echo "\n Generating the executable " $@
	$(CXX) $(OBJ) -o $(TARGET) -lpthread

#Generating an object file from a C file having the same name
%.o: %.c $(HEAD)
	@echo "\n Generating " $@ " from " $<
	$(CXX) $(CFLAGS) -c -o $@ $<

#Cleaning the content of the current directory
clean:
	@echo "\n Cleaning temporary files"
	rm -rf *.o *~ *.so *.exe
