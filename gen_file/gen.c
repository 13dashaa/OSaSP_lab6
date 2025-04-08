#include "../structures.h"
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

double getJulian(int r) {
    time_t now = time(NULL);
    srandom(getpid() + r);
    struct tm *local = localtime(&now);
    int year = (int) random() % local->tm_year + 1900;
    int month = (int) random() % local->tm_mon + 1;
    int day = (int) random() % local->tm_mday;
    int hour = (int) random() % (local->tm_hour + 1);
    int min = (int) random() % (local->tm_min + 1);
    int sec = (int) random() % (local->tm_sec + 1);

    int a = (int) ((14 - month) / 12);
    int y = year + 4800 - a;
    int m = month + 12 * a - 3;
    double JDN = day + (int) ((153 * m + 2) / 5) + 365 * y + (int) (y / 4) - (int) (y / 100) + (int) (y / 400) - 32045;
    return JDN + (double) (hour - 12) / 24 + (double) min / 1440 + (double) sec / 86400 - 2400000.5;
}

void create_file(char *file_path, long filesize) {
    int fd;
    if (!(fd = open(file_path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXO | S_IRWXU | S_IRWXG))) {
        fprintf(stderr, "Cannot create file\n");
        exit(-4);
    }
    index_hdr_s hdr;
    hdr.records = filesize;
    for (size_t i = 0; i < hdr.records; i++) {
        hdr.idx[i].recno = i + 1;
        hdr.idx[i].time_mark = getJulian((int) i);
    }
    write(fd, &hdr, sizeof(uint64_t) + filesize * sizeof(index_s));
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc < 1) {
        fprintf(stderr, "bad command line parameters\n");
        return -1;
    }
    long filesize = strtol(argv[1], NULL, 10);
    if (filesize == 0) {
        fprintf(stderr, "bad command line parameters\n");
        return -1;
    }
    if (filesize % 256 != 0) {
        fprintf(stderr, "file size must be divisible by 256\n");
        return -1;
    }
    create_file("../gen.txt", filesize);
    printf("file generated\n");
    return 0;
}