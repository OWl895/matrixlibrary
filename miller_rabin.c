#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <gmp.h>

int miller_rabin_test(mpz_t n, int k) {
    if (mpz_cmp_ui(n, 2) < 0) return 0;
    if (mpz_cmp_ui(n, 2) == 0) return 1;
    if (mpz_cmp_ui(n, 3) == 0) return 1;
    if (mpz_even_p(n)) return 0;
    
    int small_primes[] = {3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
    int num_small = sizeof(small_primes)/sizeof(small_primes[0]);
    
    for(int i=0; i<num_small; i++) {
        if(mpz_cmp_ui(n, small_primes[i])==0) return 1;
        if(mpz_divisible_ui_p(n, small_primes[i])) return 0;
    }
    
    mpz_t n_minus_1, d;
    mpz_init(n_minus_1);
    mpz_init(d);
    
    mpz_sub_ui(n_minus_1, n, 1);
    mpz_set(d, n_minus_1);
    
    int s=0;
    while(mpz_even_p(d)) {
        mpz_fdiv_q_2exp(d, d, 1);
        s++;
    }
    
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, time(NULL));
    
    mpz_t a, x, n_minus_2;
    mpz_init(a);
    mpz_init(x);
    mpz_init(n_minus_2);
    mpz_sub_ui(n_minus_2, n, 2);
    
    int result=1;
    
    for(int i=0; i<k; i++) {
        mpz_urandomm(a, state, n_minus_2);
        mpz_add_ui(a, a, 2);
        
        mpz_powm(x, a, d, n);
        
        if(mpz_cmp_ui(x,1)==0 || mpz_cmp(x,n_minus_1)==0) continue;
        
        int composite=1;
        for(int r=1; r<s; r++) {
            mpz_powm_ui(x, x, 2, n);
            if(mpz_cmp(x, n_minus_1)==0) {
                composite=0;
                break;
            }
        }
        if(composite) {
            result=0;
            break;
        }
    }
    
    mpz_clear(n_minus_1);
    mpz_clear(d);
    mpz_clear(a);
    mpz_clear(x);
    mpz_clear(n_minus_2);
    gmp_randclear(state);
    
    return result;
}

void read_numbers(const char* filename, mpz_t** numbers, char*** types, int* count) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Ошибка открытия файла %s\n", filename);
        exit(1);
    }
    
    *count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    
    while ((read = getline(&line, &len, file)) != -1) {
        if (read > 2) (*count)++;
    }
    
    rewind(file);
    
    *numbers = (mpz_t*)malloc(*count * sizeof(mpz_t));
    *types = (char**)malloc(*count * sizeof(char*));
    
    for (int i = 0; i < *count; i++) {
        mpz_init((*numbers)[i]);
        (*types)[i] = (char*)malloc(20 * sizeof(char));
        
        read = getline(&line, &len, file);
        if (read != -1) {
            if (line[read-1] == '\n') line[read-1] = '\0';
            
            char *token = strtok(line, " ");
            if (token) {
                strcpy((*types)[i], token);
                token = strtok(NULL, " ");
                if (token) {
                    mpz_set_str((*numbers)[i], token, 10);
                } else {
                    printf("Ошибка %d\n", i+1);
                    exit(1);
                }
            } else {
                printf("Пустая строка %d\n", i+1);
                exit(1);
            }
        }
    }
    
    free(line);
    fclose(file);
}

int main() {
    clock_t start_time, end_time;
    double total_time;
    
    int k = 20;
    
    printf("Параметры теста:\n");
    printf("  Количество итераций: %d\n", k);
    printf("  Вероятность ошибки: 2^(-%d) ≈ 10^(-%.1f)\n\n", 2*k, 2*k*0.30103);
    
    mpz_t* numbers;
    char** types;
    int count;
    
    read_numbers("numbers.txt", &numbers, &types, &count);
    printf("Загружено чисел: %d\n\n", count);
    
    int correct_primes = 0, correct_composites = 0;
    int error_count = 0;
    
    start_time = clock();
    
    for (int i = 0; i < count; i++) {
        int is_prime = miller_rabin_test(numbers[i], k);
        int expected_prime = (strcmp(types[i], "prime") == 0);
        
        if (expected_prime && is_prime) {
            correct_primes++;
        } else if (!expected_prime && !is_prime) {
            correct_composites++;
        } else {
            error_count++;
        }
        
        printf("%d. Ожидается: %-8s | Результат: %-8s", 
               i+1, types[i], is_prime ? "простое" : "составное");
        
        if ((expected_prime && !is_prime) || (!expected_prime && is_prime)) {
            printf("ОШИБКА!\n");
        } else {
            printf("\n");
        }
    }
    
    end_time = clock();
    total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    printf("Всего чисел: %d\n", count);
    printf("Простых: %d\n", correct_primes + (error_count > 0 ? 0 : 0));
    printf("Составных: %d\n\n", correct_composites + (error_count > 0 ? 0 : 0));
    
    printf("Правильно определено:\n");
    printf("  - Простые: %d/%d (%.2f%%)\n", 
           correct_primes, correct_primes + (error_count > 0 ? 0 : 0),
           (correct_primes > 0) ? 
           (float)correct_primes * 100 / (correct_primes) : 0);
    printf("  - Составные: %d/%d (%.2f%%)\n",
           correct_composites, correct_composites + (error_count > 0 ? 0 : 0),
           (correct_composites > 0) ?
           (float)correct_composites * 100 / (correct_composites) : 0);
    
    printf("\nОшибок: %d\n", error_count);
    printf("\nОбщая точность: %.2f%%\n", 
           (float)(correct_primes + correct_composites) * 100 / count);
    printf("\nВремя выполнения: %.2f секунд\n", total_time);
    printf("Среднее время на число: %.4f секунд\n", total_time / count);
    
    for (int i = 0; i < count; i++) {
        mpz_clear(numbers[i]);
        free(types[i]);
    }
    free(numbers);
    free(types);
    
    return 0;
}
