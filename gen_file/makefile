CC = gcc
CFLAGS=-W -Wall -Wno-unused-parameter -Wno-unused-variable -std=gnu99 -pedantic -pthread
.PHONY:  build gen clean
build: gen
gen: makefile
	$(CC) $(CFLAGS) gen.c -o gen
clean:
	rm gen