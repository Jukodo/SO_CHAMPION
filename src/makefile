CC=gcc
CFLAGS=-I.
DEPS=utils/utils.h
OBJ=Server.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

Server: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o Server

run:
	./Server