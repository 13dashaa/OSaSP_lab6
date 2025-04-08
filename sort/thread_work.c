#include <memory.h>
#include <sys/stat.h>
#include <pthread.h> 
#include "thread_work.h"

index_hdr_s *file_arr;
pthread_barrier_t barrier;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
long blocks_to_merge;
long file_records = 0, file_offset = 0;


void file_to_mem(char *filename, long memsize, long offset) {
    int fd;
    if ((fd = open(filename, O_RDWR)) < 0) {
        fprintf(stderr, "Can't open file\n");
        exit(-1);
    }
    ptr = mmap(0, memsize * sizeof(index_s) + sizeof(uint64_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, (int) offset);
    printf("file in memory\n");
    file_arr = (index_hdr_s *) ptr;
    close(fd);
}

void start_sorting(size_t amount, long blocks, long memsize, char *filename) {
    struct stat st;
    stat("../gen.txt", &st);
    file_records = (st.st_size - sizeof(uint64_t)) / sizeof(index_s);
    pthread_barrier_init(&barrier, NULL, amount + 1);
    for (size_t i = 0; i < amount; i++) {
        attrs *args = (attrs *) malloc(sizeof(attrs));
        args->block_size = memsize / blocks;
        args->blocks = blocks;
        args->t_num = i + 1;
        args->buf_adr = ptr;
        pthread_create(&threads[count_thread], NULL, (void *(*)(void *)) execute, args);
        count_thread++;
    }
    while (file_records > file_offset) {
        blocks_to_merge = blocks;
        block_maps = (block_map *) malloc(blocks * sizeof(block_map));
        for (size_t i = 0; (long) i <= blocks; i++) {
            block_maps[i].block = i;
            if ((long unsigned int) i < amount)
                block_maps[i].isBusy = 1;
            else
                block_maps[i].isBusy = 0;
        }
        file_to_mem(filename, memsize, file_offset * sizeof(index_s));
        attrs *args = (attrs *) malloc(sizeof(attrs));
        args->block_size = memsize / blocks;
        args->blocks = blocks;
        args->t_num = 0;
        args->buf_adr = ptr;
        pthread_barrier_wait(&barrier);
        pthread_barrier_wait(&barrier);
        sort_blocks(args);
        merging(args);
        pthread_barrier_wait(&barrier);
        merge_blocks(args->block_size * args->blocks, 2, 0);
        file_offset += memsize;
        free(args);
    }

    pthread_barrier_wait(&barrier);
    if (msync(ptr, memsize * sizeof(index_s) + sizeof(uint64_t), MS_SYNC) < 0) {
        fprintf(stderr, "syncing failed\n");
    } else
        fprintf(stdout, "synced\n");
    munmap(ptr, memsize * sizeof(index_s) + sizeof(uint64_t));
    pthread_barrier_wait(&barrier);
    for (size_t i = 0; i < amount; i++) {
        pthread_cancel(threads[i]);
        pthread_join(threads[i], NULL);
    }
}

void *execute(attrs *args) {
    printf("thread #%zu created\n", pthread_self());
    while (file_records > file_offset) {
        pthread_barrier_wait(&barrier);
        if (file_records <= file_offset)
            break;
        pthread_barrier_wait(&barrier);
        sort_blocks(args);
        merging(args);
        pthread_barrier_wait(&barrier);
    }
    printf("thread canceled #%zu\n", args->t_num);
    pthread_barrier_wait(&barrier);
    free(args);
    return NULL;
}

void sort_blocks(attrs *args) {
    printf("sorting block #%d by %lu\n", block_maps[args->t_num].block, pthread_self());
    qsort(file_arr->idx + ((args->block_size) * args->t_num), args->block_size, sizeof(index_s), compare);
    for (size_t i = 0; (long) i < args->blocks; i++) {
        pthread_mutex_lock(&mutex);
        if (!block_maps[i].isBusy) {
            block_maps[i].isBusy = 1;
        } else {
            pthread_mutex_unlock(&mutex);
            continue;
        }
        pthread_mutex_unlock(&mutex);
        printf("sorting block #%d by %lu\n", block_maps[i].block, pthread_self());
        qsort(file_arr->idx + ((args->block_size) * block_maps[i].block), args->block_size, sizeof(index_s), compare);
    }
    pthread_barrier_wait(&barrier);
}

int compare(const void *a, const void *b) {
    index_s *indexA = (index_s *) a;
    index_s *indexB = (index_s *) b;
    double cmp = indexA->time_mark - indexB->time_mark;
    return cmp > 0 ? 1 : cmp < 0 ? -1 : 0;
}

void merging(attrs *args) {
    while (blocks_to_merge > 2) {
        pthread_barrier_wait(&barrier);
        if (args->t_num == 0) {
            blocks_to_merge /= 2;

            for (int i = 0; i < blocks_to_merge; i++) {
                block_maps[i].isBusy = 0;
            }
        }
        pthread_barrier_wait(&barrier);
        for (size_t i = 0; (long) i < blocks_to_merge; i++) {
            pthread_mutex_lock(&mutex);
            if (!block_maps[i].isBusy) {
                block_maps[i].isBusy = 1;
                pthread_mutex_unlock(&mutex);
                merge_blocks(args->block_size * args->blocks / 2, blocks_to_merge, i * 2);
            } else {
                pthread_mutex_unlock(&mutex);
                continue;
            }
        }
    }
}

void merge_blocks(long memsize, long blocks, long block_num) {
    index_s *idx1 = (index_s *) malloc(memsize / blocks * sizeof(index_s));
    index_s *idx2 = (index_s *) malloc(memsize / blocks * sizeof(index_s));
    memcpy(idx1, file_arr->idx + (memsize / blocks) * (block_num), (memsize / blocks) * sizeof(index_s));
    memcpy(idx2, file_arr->idx + (memsize / blocks) * (block_num + 1), (memsize / blocks) * sizeof(index_s));
    size_t i = 0, j = 0, k = (memsize / blocks) * (block_num), n = (memsize / blocks);
    printf("merging blocks #%ld #%ld by %lu mem %ld\n", block_num, block_num + 1, pthread_self(), memsize / blocks);
    while (i < n && j < n) {
        if (idx1[i].time_mark < idx2[j].time_mark) {
            file_arr->idx[k++] = idx1[i++];
        } else {
            file_arr->idx[k++] = idx2[j++];
        }
    }
    while (i < n) {
        file_arr->idx[k++] = idx1[i++];
    }
    while (j < n) {
        file_arr->idx[k++] = idx2[j++];
    }
}