CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -O2 -lm
TARGET=echo_server
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
PORT=5000

.PHONY: all run clean

all: $(TARGET)

run: $(TARGET)
	./$< $(PORT)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.o $(TARGET)
