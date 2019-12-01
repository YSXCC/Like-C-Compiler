CC=g++
EXE=cit
RM=del		#windows下删除的命令
SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)
CXXFLAGS=-g -Wno-deprecated -DDEBUG	#传入参数DEBUG
all:$(EXE)
$(EXE):$(OBJ)
	$(CC) $(OBJ) -o $(EXE)
clean:
	$(RM) $(EXE) $(OBJ)
	