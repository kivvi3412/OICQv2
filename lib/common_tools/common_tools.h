#include <stdio.h>
#include <string.h>
//#include <openssl/md5.h>


//void md5_string_with_salt(const char *str, const char *salt, char output[MD5_DIGEST_LENGTH*2+1]) {
//    char salted_str[256];
//    strncpy(salted_str, str, sizeof(salted_str));
//    strncat(salted_str, salt, sizeof(salted_str) - strlen(salted_str) - 1);
//    unsigned char digest[MD5_DIGEST_LENGTH];
//    MD5((unsigned char*)salted_str, strlen(salted_str), digest);
//    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
//        sprintf(&output[i*2], "%02x", (unsigned int)digest[i]);
//    }
//    output[MD5_DIGEST_LENGTH*2] = '\0'; // add null terminator
//}


void md5_string_with_salt(const char *str, const char *salt, char output[33*2+1]) {
    // 先不用md5， 先直接字符串拼接输出
    char salted_str[256];
    strncpy(salted_str, str, sizeof(salted_str));
    strncat(salted_str, salt, sizeof(salted_str) - strlen(salted_str) - 1);
    strcpy(output, salted_str);
    output[strlen(salted_str)] = '\0';
}

