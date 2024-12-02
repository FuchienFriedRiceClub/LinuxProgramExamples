# simple makefile template
CC=gcc
CFLAG=-g -save-temps -fstack-protector -z noexecstack -z relro -z now
# compiler options
LDFLAG=
# link options
LIB_DIR=
LIB=
BASE_DIR=
INC_DIR=
INC=

SRC=main.c
OBJ_DIR=./obj
NAME=null_ptr_dereference
OBJ=$(patsubst %.c, $(OBJ_DIR)/%.o, $(SRC))
# $(patsubst replaced value, replace value, files list)

all: ObjDirCreate $(NAME)

ObjDirCreate:
	mkdir -p $(OBJ_DIR)

$(NAME): $(OBJ)
	$(CC) $^ -o $@ $(CFLAG) $(INC) $(LIB) $(LDFLAG)

$(OBJ_DIR)/%.o: %.c
	$(CC) -c $^ -o $@ $(CFLAG) $(INC) $(LDFLAG)

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(NAME)

# 1 : 2
# $@: $^
