.SUFFIXES: .c .cpp .obj

CC=gcc
CFLAGS=-g -Wall -O2 -fPIC 
INCL=-I. -I../include
LIBS=-lm 




OUT_DIR=../../bin
OBJ_DIR=../../obj
SRC_DIR=../../src

OUT=$(OUT_DIR)/logic


SRC=$(shell find $(SRC_DIR) -name '*.cpp' -type f)
SRC+=$(shell find $(SRC_DIR/card_impl) -name '*.cpp' -type f)
SRC+=$(shell find $(SRC_DIR/hero_impl) -name '*.cpp' -type f)

OBJS=$(SRC:.cpp=.obj) 
DEPS=$(SRC:.cpp=.dep)


all:$(OUT)


$(OUT): $(OBJS)
	@echo generating $@ ...
	@$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
	@echo done.

.cpp.obj:
	@echo compiling $@ ...
	@$(CC) -c -o $@ $< $(CFLAGS) $(INCL) -MT $@ -MF `echo $@ | sed -e 's,\.obj$$,.dep,'` -MD -MP

clean:
	-rm -rf $(OBJS)
	-rm -rf $(DEPS)
	-rm -rf $(OUT)



-include $(DEPS)





