CC = g++
EXEC = ./bin/tsloader
INCLUDE = ./include
CPP = ./src/*.cpp 

all:
	@-mkdir -p bin	
	@-$(CC) $(CPP) -o $(EXEC) -I $(INCLUDE)

clean:
	@-rm -r ./bin

run:
	@-$(EXEC) $(filter-out $@,$(MAKECMDGOALS))

