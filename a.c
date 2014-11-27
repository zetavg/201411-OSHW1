#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_WORD_SIZE 1024
#define SHM_NAME "ab_shm_8617ffee"
#define OPEN_FLAG O_RDWR|O_CREAT
#define OPEN_MODE 0777


int main() {
    int fd = shm_open(SHM_NAME, OPEN_FLAG, OPEN_MODE);
    if (fd == -1) { printf("shm failed\n"); return -1; }
    ftruncate(fd, MAX_WORD_SIZE);
    char* ptr = mmap(0, MAX_WORD_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == NULL) { printf("ptr failed\n"); return -1; }
    char inpt_buf[MAX_WORD_SIZE];

    do {
        printf("> ");
        scanf("%s", inpt_buf);
        sprintf(ptr, "%s", inpt_buf);

        while (strlen(ptr) > 0) {
            usleep(1000);
        }
    } while (1);
}
