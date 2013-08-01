.SUFFIXES: .c .o 

CC=gcc
AR=ar rcu
RANLIB=ranlib
CP=cp

CFLAGS=-g -Wall -ansi -fPIC -DTOLUA_SCRIPT_RUN
INCL=-I../../include -I../../../lua-5.1/include 

LIB=-L../../lib  -ltolua++ -llua -lm  -ldl -L../../../lua-5.1/lib

SRC=tolua.c
OBJS=$(SRC:.c=.o)

EXE=tolua++

all: exe

exe:$(EXE)



.c.o:
	$(CC) -c -o $@ $(CFLAGS)   $(INCL) $<

$(EXE): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIB)
	$(CP)  $@  ../../bin

clean:
	-rm -rf $(EXE) $(OBJS)

tolua.o: tolua.c ../../include/tolua++.h \
 ../../../lua-5.1/include/lua.h ../../../lua-5.1/include/luaconf.h \
 ../../../lua-5.1/include/lauxlib.h ../../../lua-5.1/include/lua.h




