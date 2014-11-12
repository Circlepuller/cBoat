CC=gcc
CFLAGS=-std=gnu11 -pedantic -Wall -Iinclude -DIRC_DEBUG -DIRC_BUFLEN=512

all:
	$(CC) $(CFLAGS) -o cboat src/*.c
