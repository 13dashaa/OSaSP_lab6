CC = gcc
CFLAGS=-W -Wall -Wno-unused-parameter -Wno-unused-variable -std=gnu99 -pedantic -pthread
.PHONY:  build sort clean
build: sort
sort: makefile
	$(CC) $(CFLAGS) sort.c thread_work.c -o sort
clean:
	rm sort