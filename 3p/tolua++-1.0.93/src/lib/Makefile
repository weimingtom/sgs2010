CC=gcc
CFLAGS=-fPIC  -Wall -O3
INCL=-I. -I../../include
LIBS=-llua -lm -ldl 
OBJS=tolua_event.o tolua_is.o tolua_map.o tolua_push.o tolua_to.o
OUT=libtolua++.a

lib: $(OUT)


$(OUT): $(OBJS)
	ar cvr $@ $^ 
	ranlib $@

.c.o: 
	$(CC) -c -o $@ $< $(INCL) $(CFLAGS)

clean: 
	-rm -rf $(OBJS) $(OUT)

install:
	if [ ! -e ../../lib ]; then mkdir -p ../../lib; fi
	cp $(OUT) ../../lib 
