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

/* Hashtable Dictionary (for word -> occurance) Data Structure */

typedef struct wlist {
    struct wlist *next;
    char *word;
    int occurance;
} wlist;

unsigned hash(char *s) {
    unsigned hashval;
    for (hashval = 0; *s != '\0'; s++)
        hashval = *s + 31 * hashval;
    return hashval % 128;
}

wlist *wo_hashtab[128];

wlist *wo_lookup(char *s) {
    wlist *wptr;
    for (wptr = wo_hashtab[hash(s)]; wptr != NULL; wptr = wptr->next)
        if (strcmp(s, wptr->word) == 0)
            return wptr;
    return NULL;
}

int wo_get(char *s) {
    wlist *wptr = wo_lookup(s);
    if (wptr == NULL) {
        return 0;
    } else {
        return wptr->occurance;
    }
}

int wo_plus(char *s) {
    wlist *wptr = wo_lookup(s);
    if (wptr == NULL) {
        wptr = (struct wlist *) malloc(sizeof(*wptr));
        int hashval = hash(s);
        wptr->word = strdup(s);
        wptr->occurance = 1;
        wptr->next = wo_hashtab[hashval];
        wo_hashtab[hashval] = wptr;
        return 1;
    } else {
        return wptr->occurance++;
    }
}

void wo_print_all() {
    wlist *wptr;
    int i;
    for (i=0; i<128; i++) {
        for (wptr = wo_hashtab[i]; wptr != NULL; wptr = wptr->next) {
            printf("%s %d\n", wptr->word, wptr->occurance);
        }
    }
}


int main() {
    int fd = shm_open(SHM_NAME, OPEN_FLAG, OPEN_MODE);
    if (fd == -1) { printf("shm failed\n"); return -1; }
    ftruncate(fd, MAX_WORD_SIZE);
    char* ptr = mmap(0, MAX_WORD_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == NULL) { printf("ptr failed\n"); return -1; }

    do {
        if (strlen(ptr) > 0) {

            if (ptr[0] == '%') {
                char* word = ptr + 1;
                int o = wo_get(word);
                if (o <= 0) {
                    printf("no\n");
                } else {
                    printf("yes\n");
                }

            } else if (ptr[0] == '$') {
                wo_print_all();

            } else {
                wo_plus(ptr);
            }
        }

        sprintf(ptr, "");
        usleep(1000);
    } while (1);
}
