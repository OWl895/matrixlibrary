#include <stdio.h>
#include <gmp.h>
#include <mpfr.h>
#include <math.h>

void get_fib(mpz_t f46, mpz_t f47) {
    mpz_fib_ui(f46, 46);
    mpz_fib_ui(f47, 47);
}

int main() {
    mpz_t x, y, x3, y3, x5, y5, x6, y6, t2, t3, t4, den;
    mpz_inits(x, y, x3, y3, x5, y5, x6, y6, t2, t3, t4, den, NULL);

    get_fib(x, y);

    // y^6 - 3xy^5 + 5x^3y^3 - 3x^5y - x^6
    mpz_pow_ui(y6, y, 6);
    mpz_pow_ui(y5, y, 5);
    mpz_mul(t2, x, y5);
    mpz_mul_ui(t2, t2, 3);

    mpz_pow_ui(x3, x, 3);
    mpz_pow_ui(y3, y, 3);
    mpz_mul(t3, x3, y3);
    mpz_mul_ui(t3, t3, 5);

    mpz_pow_ui(x5, x, 5);
    mpz_mul(t4, x5, y);
    mpz_mul_ui(t4, t4, 3);

    mpz_pow_ui(x6, x, 6);

    mpz_sub(den, y6, t2);
    mpz_add(den, den, t3);
    mpz_sub(den, den, t4);
    mpz_sub(den, den, x6);

    gmp_printf("Числа Фибоначчи: x = %Zd, y = %Zd\n", x, y);

    mpf_t gmp_res;
    mpf_init2(gmp_res, 256);
    mpf_set_z(gmp_res, den);
    mpf_ui_div(gmp_res, 1, gmp_res); // Вычисляем 1 / den
    gmp_printf("Результат (GMP): %.1Ff\n", gmp_res);
    mpf_clear(gmp_res);

    double dx = mpz_get_d(x);
    double dy = mpz_get_d(y);
    double d_den = pow(dy, 6) - 3*dx*pow(dy, 5) + 5*pow(dx, 3)*pow(dy, 3) - 3*pow(dx, 5)*dy - pow(dx, 6);
    printf("Результат (double): %e\n", 1.0 / d_den);

    mpfr_t mx, my, m_den, m_res;
    mpfr_inits2(256, mx, my, m_den, m_res, (mpfr_ptr)0);

    mpfr_set_z(mx, x, MPFR_RNDN);
    mpfr_set_z(my, y, MPFR_RNDN);

    mpfr_t term;
    mpfr_init2(term, 256); 

    mpfr_pow_ui(m_den, my, 6, MPFR_RNDN);

    mpfr_pow_ui(term, my, 5, MPFR_RNDN);
    mpfr_mul(term, term, mx, MPFR_RNDN);
    mpfr_mul_ui(term, term, 3, MPFR_RNDN);
    mpfr_sub(m_den, m_den, term, MPFR_RNDN);

    mpfr_pow_ui(term, mx, 3, MPFR_RNDN);
    mpfr_t ty3; 
    mpfr_init2(ty3, 256);  
    mpfr_pow_ui(ty3, my, 3, MPFR_RNDN);
    mpfr_mul(term, term, ty3, MPFR_RNDN);
    mpfr_mul_ui(term, term, 5, MPFR_RNDN);
    mpfr_add(m_den, m_den, term, MPFR_RNDN);

    mpfr_pow_ui(term, mx, 5, MPFR_RNDN);
    mpfr_mul(term, term, my, MPFR_RNDN);
    mpfr_mul_ui(term, term, 3, MPFR_RNDN);
    mpfr_sub(m_den, m_den, term, MPFR_RNDN);

    mpfr_pow_ui(term, mx, 6, MPFR_RNDN);
    mpfr_sub(m_den, m_den, term, MPFR_RNDN);

    mpfr_ui_div(m_res, 1, m_den, MPFR_RNDN);

    mpfr_printf("Результат (MPFR): %.1Rf\n", m_res);

    // Очистка
    mpz_clears(x, y, x3, y3, x5, y5, x6, y6, t2, t3, t4, den);
    mpfr_free_cache();

    return 0;
}
