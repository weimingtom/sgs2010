CC=gcc
CFLAGS=-fPIC  -Wall -O3
INCL=-I. -I../../include
LIBS=-llua -lm -ldl -ltolua++   -L../../lib
OBJS=tolua.o toluabind.o
OUT=tolua++

bin: $(OUT)


$(OUT): $(OBJS)
	$(CC) -o $@ $^  $(LIBS) $(CFLAGS)

.c.o: 
	$(CC) -c -o $@ $< $(INCL) $(CFLAGS)

clean:
	-rm -f *.o $(OUT)

install:
	if [ ! -e ../../bin ]; then mkdir -p ../../bin; fi
	cp $(OUT) ../../bin
	
