DIR_SRC = ./src
DIR_BIN = ./bin

OBJ_C = $(wildcard ${DIR_SRC}/*.c)
OBJ_O = $(patsubst %.c,${DIR_BIN}/%.o,$(notdir ${OBJ_C}))

TARGET = bmp2array
#BIN_TARGET = ${DIR_BIN}/${TARGET}

CC = gcc

DEBUG = -g -O0 -Wall
CFLAGS += $(DEBUG)

${TARGET}:${OBJ_O}
	$(CC) $(CFLAGS) $(OBJ_O) -o $@ 

${DIR_BIN}/%.o : $(DIR_SRC)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ 
	
clean :
	$(RM) $(DIR_BIN)/*.* $(TARGET) $(DIR_SRC)/.*.sw?
