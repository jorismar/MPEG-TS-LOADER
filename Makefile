CC = g++
EXEC = ./bin/tsloader
INCLUDE = ./include
CPP = ./src/*.cpp 

all:
	@-$(CC) $(CPP) -o $(EXEC) -I $(INCLUDE)

clean:
	@-rm -r ./bin/*

run:
	@-$(EXEC)

