CC=gcc
CFLAGS=-std=c99 -pedantic -O3 -Wall -Iinclude -DIRC_DEBUG -DIRC_BUFLEN=512

all:
	$(CC) $(CFLAGS) -o cboat src/*.c
