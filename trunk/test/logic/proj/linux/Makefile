.SUFFIXES: .c .obj .h .pkg

CC=gcc
CFLAGS=-g -Wall -O2 -fPIC 
INCL=-I. -I../include -I../../../../3p/lua-5.1/include -I../../../../3p/tolua++-1.0.93/include
LIBS=-L../../../../3p/lua-5.1/lib -L../../../../3p/tolua++-1.0.93/lib -ltolua++ -llua -lreadline -lhistory -lncurses -ldl -lm -lstdc++

TOLUA=../../../3p/tolua++-1.0.93/bin/tolua++



OUT_DIR=../../bin
OBJ_DIR=../../obj
SRC_DIR=../../src
PKG_DIR=../../pkg

OUT=$(OUT_DIR)/logic


SRC=$(shell ls $(SRC_DIR)/*.cpp)
#SRC+=$(shell ls $(SRC_DIR)/card_impl/*.cpp)
#SRC+=$(shell ls $(SRC_DIR)/hero_impl/*.cpp)

PKG=$(shell ls $(PKG_DIR)/*.pkg)

SRC+= $(PKG:.pkg=.cpp)


OBJS=$(SRC:.cpp=.obj) 
DEPS=$(SRC:.cpp=.dep)


all:$(OUT)


$(OUT): $(OBJS)
	@echo generating $@ ...
	@$(CC) -o $@   $^ $(CFLAGS) $(LIBS)
	@echo done.

$(OBJS):%.obj:%.cpp
	@echo compiling $< ...
	@$(CC) -c -o $@  $< $(CFLAGS) $(INCL) -MT $@ -MF `echo $@ | sed -e 's,\.obj$$,.dep,'` -MD -MP

../../pkg/lua_export.cpp: ../../pkg/lua_export.pkg
	@echo tolua gen $@ ...
	@cd $(PKG_DIR) && $(TOLUA) -o lua_export.cpp -H lua_export.h -n game lua_export.pkg && cd -

../../pkg/lua_export.h: ../../pkg/lua_export.pkg

clean:
	-rm -rf $(OBJS)
	-rm -rf $(DEPS)
	-rm -rf $(OUT)




-include $(DEPS)





