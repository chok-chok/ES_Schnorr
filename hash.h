#ifndef HASH_UTILS_H
#define HASH_UTILS_H

#include <openssl/evp.h>
#include <openssl/sha.h>

#define HASH_OUTPUT_SIZE 65

void hash_string(const char *input, unsigned char *output, const EVP_MD *md);

int hash_file(const char *filename);


void hash_to_hex_string(unsigned char *hash, size_t length, char *output);

extern char global_hash_output[HASH_OUTPUT_SIZE];

#endif 
