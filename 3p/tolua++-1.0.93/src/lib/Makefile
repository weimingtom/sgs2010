.SUFFIXES: .c .o 

CC=gcc
AR=ar rcu
RANLIB=ranlib
CP=cp

CFLAGS=-g -Wall -ansi -fPIC
INCL=-I../../include -I../../../lua-5.1/include 

LIB=libtolua++.a

SRC=tolua_event.c tolua_is.c tolua_map.c tolua_push.c tolua_to.c
OBJS=$(SRC:.c=.o)

all:lib

lib:$(LIB)



.c.o:
	$(CC) -c -o $@ $(CFLAGS)   $(INCL) $<

$(LIB): $(OBJS)
	$(AR) $@ $?
	$(RANLIB) $@ 
	$(CP)  $@  ../../lib/

clean:
	-rm -r $(LIB) $(OBJS)

tolua_event.o: tolua_event.c ../../include/tolua++.h \
 ../../../lua-5.1/include/lua.h ../../../lua-5.1/include/luaconf.h \
 ../../../lua-5.1/include/lauxlib.h ../../../lua-5.1/include/lua.h
tolua_is.o: tolua_is.c ../../include/tolua++.h \
 ../../../lua-5.1/include/lua.h ../../../lua-5.1/include/luaconf.h \
 ../../../lua-5.1/include/lauxlib.h ../../../lua-5.1/include/lua.h
tolua_map.o: tolua_map.c ../../include/tolua++.h \
 ../../../lua-5.1/include/lua.h ../../../lua-5.1/include/luaconf.h \
 ../../../lua-5.1/include/lauxlib.h ../../../lua-5.1/include/lua.h \
 tolua_event.h
tolua_push.o: tolua_push.c ../../include/tolua++.h \
 ../../../lua-5.1/include/lua.h ../../../lua-5.1/include/luaconf.h \
 ../../../lua-5.1/include/lauxlib.h ../../../lua-5.1/include/lua.h
tolua_to.o: tolua_to.c ../../include/tolua++.h \
 ../../../lua-5.1/include/lua.h ../../../lua-5.1/include/luaconf.h \
 ../../../lua-5.1/include/lauxlib.h ../../../lua-5.1/include/lua.h




