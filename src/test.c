#include "stdio.h"
#include "socket_tools/struct_json_transfer.h"


int main() {
    char *t = "a9df10c94eea642c5de37bdb32a12c53a39150a96703a95f4fc9dd8e792fd2ed";
    if (strcmp(check_token(t), "@#@failed@#@") == 0) {
        printf("token is invalid\n");
    } else {
        printf("%s\n", check_token(t));
    }

}