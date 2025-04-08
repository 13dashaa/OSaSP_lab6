#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include "../structures.h"

int main(int argc, char* argv[])
{
    index_hdr_s hdr;
    int fd;
    if (!(fd = open("../gen.txt", O_RDWR , S_IRWXO | S_IRWXU | S_IRWXG))) {
        fprintf(stderr, "Cannot open file\n");
        exit(-4);
    }
    long size= strtol(argv[1],NULL,10);
    read(fd,&hdr,sizeof(uint64_t)+size* sizeof( index_s));
    for(int i=0;i<size;i++)
    {
        printf("recno - %lu time_mark - %f \n",hdr.idx[i].recno,hdr.idx[i].time_mark);
    }
    return 0;
}