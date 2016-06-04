#my general makefile template
CXX = gcc
CFLAGS = -Wall #-fpic

#Application-related macros
SRC = main.c voie.c aiguillage.c train.c list.c
OBJ = $(SRC:.c=.o)
HEAD= $(SRC:.c=.o) structures.h
TARGET = aiguillage.exe

#Running the program
run: $(TARGET)
	@echo "\n Executing the executable " $(TARGET)
	./$(TARGET)

build: $(TARGET)

#Generating the executable
$(TARGET): $(OBJ) $(HEAD)
	@echo "\n Generating the executable " $@
	$(CXX) $(OBJ) -o $(TARGET) -lpthread

#Generating an object file from a C file having the same name
%.o: %.c %.h
	@echo "\n Generating " $@ " from " $<
	$(CXX) $(CFLAGS) -c -o $@ $<

#Cleaning the content of the current directory
clean:
	@echo "\n Cleaning temporary files"
	rm -rf *.o *~ *.so *.exe
