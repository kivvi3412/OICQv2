#include <openssl/sha.h>

/*
 * 加密函数, 用于生成token
 */

void sha256_string_with_salt(const char *str, const char *salt, char output[SHA256_DIGEST_LENGTH * 2 + 1]) {
    char salted_str[256];
    strncpy(salted_str, str, sizeof(salted_str));
    strncat(salted_str, salt, sizeof(salted_str) - strlen(salted_str) - 1);

    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *) salted_str, strlen(salted_str), digest);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(&output[i * 2], "%02x", (unsigned int) digest[i]);
    }
    output[SHA256_DIGEST_LENGTH * 2] = '\0'; // add null terminator
}

