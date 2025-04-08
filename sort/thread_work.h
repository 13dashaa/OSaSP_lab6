#ifndef LAB__6_THREAD_WORK_H
#define LAB__6_THREAD_WORK_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include "../structures.h"
#include <fcntl.h>

extern char *ptr;
extern block_map *block_maps;

extern pthread_t threads[8192];
extern size_t count_thread;

void start_sorting(size_t amount_threads,long blocks,long memsize,char* filename);
void file_to_mem(char* filename,long memsize,long offset);
void* execute(attrs* args);
void sort_blocks(attrs *args);
int compare(const void *a, const void *b);
void merge_blocks(long memsize, long blocks, long block_num);
void merging(attrs* args);

#endif //LAB__6_THREAD_WORK_H