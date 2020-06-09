CC=g++  -lpthread -lsqlite3 -lrt -lstdc++
ELF=main
SRC=$(shell find ./ -name '*.cpp')
OBJ=$(SRC:.cpp=.o)
$(ELF):$(OBJ)
$(OBJ):
clean:
	rm -f $(OBJ) $(ELF)
