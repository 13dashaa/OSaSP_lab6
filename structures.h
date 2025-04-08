#ifndef LAB__6_STRUCTURES_H
#define LAB__6_STRUCTURES_H

#include <stdint.h>
#include <stddef.h>

typedef struct  {
    double time_mark;
    uint64_t recno;
}index_s;

typedef struct {
    uint64_t records;
    index_s idx[100000];
}index_hdr_s;

typedef struct
{
    char* buf_adr;
    long block_size;
    long blocks;
    size_t t_num;
}attrs;

typedef struct  {
    int isBusy;
    int block;
}block_map;

#endif //LAB__6_STRUCTURES_H