#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <gmp.h>
#include <stdint.h>
#include "hash.h"

// Функция генерации рандомного числа
long int generate_chislo() {
    return 1 + rand() % 100000;
}

// Проверка простоты числа 
bool is_prime(long int num1) {
    if (num1 < 2) return false;
    if (num1 == 2) return true;
    if (num1 % 2 == 0) return false;
    for (long int i = 3; i * i <= num1; i += 2) {
        if (num1 % i == 0) return false;
    }
    return true;
}

// Быстрое возведение в степень по модулю
long int mod_pow(long int g, long int ql, long int pl) {
    long int mul = 1;
    g = g % pl;
    while (ql > 0) {
        if (ql % 2 == 1) {
            mul = (mul * g) % pl;
        }
        ql = ql / 2;
        g = (g * g) % pl;
    }
    return mul;
}

long int it_is_p() {
    long int num2;
    do {
        num2 = generate_chislo();
    } while (!is_prime(num2));
    return num2;
}
//функция проверки того, что число простое и является делителем
long int delitel(long int p_1) {
    long int num3 = p_1 - 1;
    long int divisors[10000];
    int cnt = 0;

    for (long int q = 5; q <= num3; q++) {
        if (num3 % q == 0 && is_prime(q)) {
            divisors[cnt] = q;
            cnt++;
        }
    }

    if (cnt == 0) return -1;  

    int random_index = rand() % cnt;
    return divisors[random_index];
}

long int find_a(long int qp, long int pq) {
    long int a1;
    do {
        a1 = 1 + rand() % (pq - 1); 
    } while (mod_pow(a1, qp, pq) != 1);
    return a1;
}

long int it_is_r(long int qqq) {
    long int r_1 = 1 + rand() % (qqq - 1);
    return r_1;
}

long int it_is_s(long int q_1) {
    long int s_1 = 5 + rand() % (q_1 - 1);
    return s_1;
}

// Функция для вычисления открытого ключа v
long int public_key(long int aq, long int sq, long int pw) {
    long int a_inverse = mod_pow(aq, pw - 2, pw); // вычисляем a^(p-2) mod p
    long int v1 = mod_pow(a_inverse, sq, pw);     // вычисляем (a^(-1))^s mod p
    return v1;
}

long int it_is_x(long int pe, long int ae, long int re) {
    long int x_1 = mod_pow(ae, re, pe);
    return x_1;
}

void hex_to_decimal(const char *hex, mpz_t result) {
    mpz_set_str(result, hex, 16); 
}

long int it_is_y(long int r, long int s, long int e, long int p) {
    long int y_1 = (r + s * e) % p;
    return y_1;
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <имя_файла>\n", argv[0]);
        return 1;
    }
    srand(time(0)); 
    int t = 100;
    printf("Создание подписи - 0, проверка подписи - 1\n");
    scanf("%d", &t);

    long int p = 15991, q = 5, a = 3853, v = 1, y = 2;
    //long int p = it_is_p();
    printf("Простое число p: %ld\n", p);

    //long int q = delitel(p);
    printf("Простое число q|(p-1): %ld\n", q);

    //long int a = find_a(q, p);
    printf("Число а, для которого выполняется a^q = 1(mod p): %ld\n", a);
    
    printf("\n");

    long int s = it_is_s(q);
    printf("Секретный ключ s: %ld\n", s);

    //long int v = public_key(a, s, p);
    printf("Открытый ключ v = a^(-s) mod p: %ld\n", v);
    printf("\n");

    long int r = it_is_r(q);
    //printf("Число r < q : %ld\n", r);

    long int x = it_is_x(p, a, r);
    // printf("Число х = a^r (mod p): %ld\n", x);
    // printf("Переводим X из 10-СС в 16-СС\n");
    // printf("\n");

    char mas[HASH_OUTPUT_SIZE]; 

    if (hash_file(argv[1]) == 0) {
        strcpy(mas, global_hash_output);
        //printf("Хеш файла M = H(): %s\n", mas);
    } else {
        printf("Ошибка при хешировании файла.\n");
        return 1; 
    }

    
    char result[10000];
    sprintf(result, "%s%lX", mas, x); 
    //printf("Конкатенация х и хеша файла: %s\n", result);

    unsigned char hash_output[EVP_MAX_MD_SIZE];  
    char e[EVP_MAX_MD_SIZE * 2 + 1];              
    unsigned int hash_length = 0;

    hash_string(result, hash_output, EVP_sha256());
    hash_length = EVP_MD_size(EVP_sha256());

    
    hash_to_hex_string(hash_output, hash_length, e);
    //printf("\n");
    //printf("e = H(M|x):  %s\n", e);

    mpz_t e_decimal;
    mpz_init(e_decimal); 
    hex_to_decimal(e, e_decimal);
    //gmp_printf("e в 10-ичной системе: %Zd\n", e_decimal); 

    mpz_t s_mpz, se_mod_q;
    mpz_init_set_si(s_mpz, s); 


    mpz_init(se_mod_q);
    mpz_mul(se_mod_q, e_decimal, s_mpz); 

    mpz_t q_mpz;
    mpz_init_set_si(q_mpz, q); 

    // Вычисляем (se) mod q
    mpz_mod(se_mod_q, se_mod_q, q_mpz); 
    //gmp_printf("se mod q: %Zd\n", se_mod_q); 

    // Проверка на переполнение перед преобразованием в long int
    if (mpz_cmp_si(se_mod_q, LONG_MAX) > 0 || mpz_cmp_si(se_mod_q, LONG_MIN) < 0) {
        fprintf(stderr, "Ошибка: значение se mod q превышает пределы long int.\n");
        return 1; 
    }

    long int se_long = mpz_get_si(se_mod_q);

    //long int y = (r + se_long) % q;
    //printf("y = (r + se) mod q: %ld\n", y);
    if (t == 0){
        printf("Значения подписи (e, y) = (%s, %ld)\n", e, y);
    }
    mpz_t p_mpz;
    mpz_init_set_si(p_mpz, p);

    mpz_t v_mpz, qq;
    mpz_init_set_si(v_mpz, v);  
    mpz_init(qq);                

    
    mpz_powm(qq, v_mpz, e_decimal, p_mpz);

    //gmp_printf("v^e mod p = %Zd\n", qq);
    long int qq_long = mpz_get_si(qq);

    long int a_y = mod_pow(a, y, p);

    long int x_ = (a_y*qq_long) % p;
    //printf("x' = (a^y * v^e) mod p: %ld\n", x_);

    char res[10000];
    sprintf(res, "%s%lX", mas, x_); 
    //printf("Конкатенация х' и хеша файла: %s\n", res);

 
    char e_[EVP_MAX_MD_SIZE * 2 + 1];              

    hash_string(res, hash_output, EVP_sha256());
    hash_length = EVP_MD_size(EVP_sha256());

    hash_to_hex_string(hash_output, hash_length, e_);
    //printf("\n");
    //printf("e = H(M|x'):  %s\n", e_);

    FILE *file = fopen("sig.txt", "w");
    if (file == NULL) {
        perror("Ошибка открытия файла");
        return 1;
    }

    fprintf(file, "%s\n", e);

    fclose(file);

    if (t == 1){
        if (x == x_){
            printf("ПОДПИСЬ ВЕРНА\n");
        }else { printf("подпись некорректна\n");}
    }

    mpz_clear(v_mpz);
    mpz_clear(p_mpz);
    mpz_clear(e_decimal);
    mpz_clear(qq);
    mpz_clear(q_mpz);
    return 0;
}
