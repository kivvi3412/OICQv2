#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>

char *token;
char *username_temp;
char *message_value_string;

void init_shared_memory() {
    token = mmap(NULL, 100, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (token == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    username_temp = mmap(NULL, 100, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (username_temp == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    message_value_string = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (message_value_string == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
}
