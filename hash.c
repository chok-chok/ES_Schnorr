#include <stdio.h>
#include <openssl/evp.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

#define CHUNK_SIZE 4096

char global_hash_output[HASH_OUTPUT_SIZE] = {0};

// Функция для вывода хеша в шестнадцатеричной системе
void print_hash_hex(unsigned char *hash, size_t length) {
    for (unsigned int i = 0; i < length; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}

void hash_to_hex_string(unsigned char *hash, size_t length, char *output) {
    for (unsigned int i = 0; i < length; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[length * 2] = '\0';
}

int hash_file(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Не удалось открыть файл");
        return 1;
    }

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (mdctx == NULL) {
        perror("Ошибка при создании контекста EVP_MD_CTX");
        fclose(file);
        return 1;
    }

    if (EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL) != 1) {
        perror("Ошибка при инициализации хэша SHA-256");
        EVP_MD_CTX_free(mdctx);
        fclose(file);
        return 1;
    }

    unsigned char buffer[CHUNK_SIZE];
    size_t bytes_read = 0;

    while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, file)) > 0) {
        if (EVP_DigestUpdate(mdctx, buffer, bytes_read) != 1) {
            perror("Ошибка при обновлении хэша SHA-256");
            EVP_MD_CTX_free(mdctx);
            fclose(file);
            return 1;
        }
    }

    if (ferror(file)) {
        perror("Ошибка при чтении файла");
        EVP_MD_CTX_free(mdctx);
        fclose(file);
        return 1;
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_length = 0;

    if (EVP_DigestFinal_ex(mdctx, hash, &hash_length) != 1) {
        perror("Ошибка при финализации хэша SHA-256");
        EVP_MD_CTX_free(mdctx);
        fclose(file);
        return 1;
    }

    EVP_MD_CTX_free(mdctx);
    fclose(file);

    hash_to_hex_string(hash, hash_length, global_hash_output);
    return 0;
}

void hash_string(const char *input, unsigned char *output, const EVP_MD *md) {
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    
    if (EVP_DigestInit_ex(mdctx, md, NULL) != 1) {
        perror("Ошибка при инициализации хэша");
        EVP_MD_CTX_free(mdctx);
        return;
    }
    
    if (EVP_DigestUpdate(mdctx, input, strlen(input)) != 1) {
        perror("Ошибка при обновлении хэша");
        EVP_MD_CTX_free(mdctx);
        return;
    }
    
    unsigned int hash_length = 0;
    if (EVP_DigestFinal_ex(mdctx, output, &hash_length) != 1) {
        perror("Ошибка при финализации хэша");
    }

    EVP_MD_CTX_free(mdctx);
}


// int main(int argc, char *argv[]) {
//     if (argc != 2) {
//         fprintf(stderr, "Использование: %s <имя_файла>\n", argv[0]);
//         return 1;
//     }

//     return hash_file(argv[1]);
// }
