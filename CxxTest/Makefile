# simple makefile template
CXX=g++
CFLAG=-g -save-temps
# compiler options
LDFLAG=-no-pie
# link options
LIB_DIR=
LIB=
BASE_DIR=
INC_DIR=
INC=

SRC=main.cpp
OBJ_DIR=./obj
NAME=cxx_test
OBJ=$(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRC))
# $(patsubst replaced value, replace value, files list)

all: ObjDirCreate $(NAME)

ObjDirCreate:
	mkdir -p $(OBJ_DIR)

$(NAME): $(OBJ)
	$(CXX) $^ -o $@ $(CFLAG) $(INC) $(LIB) $(LDFLAG)

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) -c $^ -o $@ $(CFLAG) $(INC)

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(NAME)

# 1 : 2
# $@: $^
