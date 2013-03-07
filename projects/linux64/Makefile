.SUFFIXES: .o .c .h .cpp .hpp .pkg

CC=g++
TOLUA=$(HOME)/proc/tolua++-1.0.93/src/bin/tolua++
INCL=-I../../source -I../../include -I$(HOME)/proc/tolua++-1.0.93/include
CFLAGS=-Wall -g -O3 -fPIC $(shell wx-config --cxxflags) $(INCL)
LIBS=-L$(HOME)/proc/tolua++-1.0.93/src/lib -ltolua++ -llua -lm $(shell wx-config --libs base,core)

SOURCE1=$(shell ls ../../source/*.cpp)
SOURCE2=$(shell ls ../../package/*.cpp)
SOURCE=$(SOURCE2:.cpp=.o) $(SOURCE1:.cpp=.o)
OBJS=$(SOURCE)
OUT=../../bin/sgs2010

all:
	echo $(SOURCE)
	echo $(OBJS)
	echo $(CFLAGS)
	echo $(LIBS)

dep: $(SOURCE)
	$(CC) -MM $(CFLAGS) $^


.cpp.o:
	$(CC) -c -o $@ $(CFLAGS) -MT$@ -MF`echo $@ | sed -e 's,\.o$$,.d,'` -MD -MP  $<


bin: $(OUT)

$(OUT): $(OBJS)
	$(CC) -o $@ $^ $(LIBS)

clean:
	-rm -Rf $(OUT)
	-rm -Rf $(OBJS)
	-rm -Rf ../.../source/*.d
	-rm -Rf ../../pakage/*.d
	echo clean done.

-include ../../source/*.d
-include ../../package/*.d


