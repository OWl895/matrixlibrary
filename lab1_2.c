#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>


typedef struct {
    mpz_t num; // Числитель
    mpz_t den; // Знаменатель
} Fraction;

void frac_init(Fraction *f) {
    mpz_init(f->num);
    mpz_init_set_ui(f->den, 1); 
}

void frac_clear(Fraction *f) {
    mpz_clear(f->num);
    mpz_clear(f->den);
}

// Cокращение дроби
void frac_reduce(Fraction *f) {
    mpz_t common_divisor;
    mpz_init(common_divisor);
    
    // Находим НОД
    mpz_gcd(common_divisor, f->num, f->den);
    
    // Делим на НОД
    mpz_divexact(f->num, f->num, common_divisor);
    mpz_divexact(f->den, f->den, common_divisor);
    
    if (mpz_sgn(f->den) < 0) {
        mpz_neg(f->num, f->num);
        mpz_neg(f->den, f->den);
    }
    
    mpz_clear(common_divisor);
}

void frac_set_si(Fraction *f, long int n, long int d) {
    if (d == 0) {
        printf("Ошибка: Знаменатель не может быть равен 0\n");
        d = 1;
    }
    mpz_set_si(f->num, n);
    mpz_set_si(f->den, d);
    frac_reduce(f);
}

void frac_add(Fraction *res, Fraction *f1, Fraction *f2) {
    mpz_t t1, t2;
    mpz_inits(t1, t2, NULL);
    
    mpz_mul(t1, f1->num, f2->den);
    mpz_mul(t2, f2->num, f1->den);
    mpz_add(res->num, t1, t2);

    mpz_mul(res->den, f1->den, f2->den);
    
    frac_reduce(res);
    mpz_clears(t1, t2, NULL);
}

void frac_sub(Fraction *res, Fraction *f1, Fraction *f2) {
    mpz_t t1, t2;
    mpz_inits(t1, t2, NULL);
    
    mpz_mul(t1, f1->num, f2->den);
    mpz_mul(t2, f2->num, f1->den);
    mpz_sub(res->num, t1, t2);
    
    mpz_mul(res->den, f1->den, f2->den);
    
    frac_reduce(res);
    mpz_clears(t1, t2, NULL);
}

void frac_mul(Fraction *res, Fraction *f1, Fraction *f2) {
    mpz_mul(res->num, f1->num, f2->num);
    mpz_mul(res->den, f1->den, f2->den);
    frac_reduce(res);
}

void frac_div(Fraction *res, Fraction *f1, Fraction *f2) {
    if (mpz_sgn(f2->num) == 0) {
        printf("Ошибка: Деление на ноль!\n");
        return;
    }
    mpz_mul(res->num, f1->num, f2->den);
    mpz_mul(res->den, f1->den, f2->num);
    frac_reduce(res);
}

void frac_print(Fraction *f) {
    gmp_printf("%Zd/%Zd", f->num, f->den);
}

void frac_fprint(FILE *fp, Fraction *f) {
    if (fp) {
        gmp_fprintf(fp, "%Zd/%Zd\n", f->num, f->den);
    }
}

void frac_print_decimal(Fraction *f, int precision) {
    mpf_t fn, fd, res;
    mpf_set_default_prec(precision * 4);
    mpf_inits(fn, fd, res, NULL);
    
    mpf_set_z(fn, f->num);
    mpf_set_z(fd, f->den);
    mpf_div(res, fn, fd);
    
    gmp_printf("%.*Ff", precision, res);
    
    mpf_clears(fn, fd, res, NULL);
}

int main() {
    Fraction a, b, res;
    frac_init(&a);
    frac_init(&b);
    frac_init(&res);

    FILE *fp = fopen("result.txt", "w");

    // Сложение (1/3 + 1/6)
    frac_set_si(&a, 1, 3);
    frac_set_si(&b, 1, 6);
    frac_add(&res, &a, &b);
    printf("Сложение: "); frac_print(&res); printf("\n");
    fprintf(fp, "Сложение: "); frac_fprint(fp, &res);

    // Вычитание (1/3 - 1/6)
    frac_sub(&res, &a, &b);
    printf("Вычитание: "); frac_print(&res); printf("\n");
    fprintf(fp, "Вычитание: "); frac_fprint(fp, &res);

    // Умножение (7612000/3 * 3/2)
    frac_set_si(&a, 7612000, 3);
    frac_set_si(&b, 3, 2);
    frac_mul(&res, &a, &b);
    printf("Умножение: "); frac_print(&res); printf("\n");
    fprintf(fp, "Умножение: "); frac_fprint(fp, &res);

    // Деление (1/2 / 1/4)
    frac_set_si(&a, 1, 2);
    frac_set_si(&b, 1, 4);
    frac_div(&res, &a, &b);
    printf("Деление: "); frac_print(&res); printf("\n"); 
    fprintf(fp, "Деление: "); frac_fprint(fp, &res);

    fclose(fp);
    frac_clear(&a);
    frac_clear(&b);
    frac_clear(&res);
    return 0;
}


