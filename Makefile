CC = gcc
CFLAGS = -ggdb -O0 -std=gnu11 -Wall -Werror
CURLC = `curl-config --cflags`
CURLL = `curl-config --libs`

all: ScAb clean

ScAb: ScAb.o
	$(CC) -o ScAb ScAb.o $(CFLAGS) $(CURLL)

ScAb.o: ScAb.c ScAb.h
	$(CC) -c ScAb.c $(CFLAGS) $(CURLC)

clean:
	rm -f *.o *.a
