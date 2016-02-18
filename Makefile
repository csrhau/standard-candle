CC=icpc
CFLAGS=-std=c++11 -O0 -Wall -Wextra -pedantic

all: candle

clean:
	rm -f candle *.o

.cc.o:
	$(CC) -c $(CFLAGS) $<

candle: candle.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)
