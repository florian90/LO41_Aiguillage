CXX = gcc
CFLAGS = -Wall #-pedantic

SRC = main.c voie.c aiguillage.c train.c structures.c
OBJ = $(SRC:.c=.o)
HEAD= voie.h aiguillage.h train.h structures.h
TARGET = aiguillage

# Running the program
run: $(TARGET)
	@echo "\n Executing the executable " $(TARGET)
	./$(TARGET)

# Building the programm
build: $(TARGET)

# Generating the executable
$(TARGET): $(OBJ) $(HEAD)
	@echo "\n Generating the executable " $@
	$(CXX) $(OBJ) -o $(TARGET) -lpthread

# Generating an object file from a C file having the same name
%.o: %.c $(HEAD)
	@echo "\n Generating " $@ " from " $<
	$(CXX) $(CFLAGS) -c -o $@ $<

# Cleaning the content of the current directory
clean:
	@echo "\n Cleaning temporary files"
	rm -rf *.o *~ *.so *.exe
