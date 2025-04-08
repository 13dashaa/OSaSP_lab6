#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include "../structures.h"
#include "thread_work.h"

pthread_t threads[8192];
size_t count_thread;

#define MAX_THREADS 8192
#define MIN_THREADS 8

char *ptr;
block_map *block_maps;

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr,
                "Not enough arguments, please run program with memory size, blocks amount, threads amount and filename\n");
        return -1;
    }

    long memsize = strtol(argv[1], NULL, 10);
    if (memsize == 0) {
        fprintf(stderr, "Memory size is zero or not a number\n");
        return -2;
    }
    if (memsize % getpagesize() != 0) {
        fprintf(stderr, "Memory size divisible by %d\n", getpagesize());
        return -3;
    }
    long blocks = strtol(argv[2], NULL, 10);
    if (blocks == 0) {
        fprintf(stderr, "Blocks amount is zero or not a number\n");
        return -2;
    }

    if ((blocks & (blocks - 1)) != 0) {
        fprintf(stderr, "Blocks amount is not power of 2\n");
        return -2;
    }

    long _threads = strtol(argv[3], NULL, 10);
    if (_threads == 0) {
        fprintf(stderr, "Threads amount is zero or not a number\n");
        return -2;
    }
    if (_threads < MIN_THREADS) {
        fprintf(stderr, "Threads amount is less than amount of kernels\n");
        return -5;
    }
    if (_threads > MAX_THREADS) {
        fprintf(stderr, "Threads amount is greater than max amount of threads\n");
        return -5;
    }
    if (_threads >= blocks) {
        fprintf(stderr, "Threads amount is greater than blocks amount\n");
        return -6;
    }
    start_sorting(_threads, blocks, memsize, argv[4]);
    return 0;
}