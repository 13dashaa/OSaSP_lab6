CC = gcc
CFLAGS=-W -Wall -Wno-unused-parameter -Wno-unused-variable -std=gnu99 -pedantic -pthread
.PHONY:  build print clean
build: print
print: makefile
	$(CC) $(CFLAGS) print.c -o print
clean:
	rm print