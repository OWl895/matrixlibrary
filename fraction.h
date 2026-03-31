#ifndef FRACTION_H
#define FRACTION_H

#include <iostream>
#include <string>
#include <stdexcept>
#include <gmp.h>

using namespace std;

class Fraction {
private:
    mpq_t val;

public:
    Fraction() { mpq_init(val); }
    
    Fraction(long n, long d = 1) {
        if (d == 0) throw runtime_error("Знаменатель равен 0");
        mpq_init(val);
        mpq_set_si(val, n, d);
        mpq_canonicalize(val);
    }

    Fraction(const Fraction& other) {
        mpq_init(val);
        mpq_set(val, other.val);
    }

    ~Fraction() { mpq_clear(val); }

    double toDouble() const { return mpq_get_d(val); }

    Fraction& operator=(const Fraction& other) {
        if (this != &other) mpq_set(val, other.val);
        return *this;
    }

    Fraction operator+(const Fraction& o) const {
        Fraction r; mpq_add(r.val, val, o.val);
        return r;
    }

    Fraction operator-(const Fraction& o) const {
        Fraction r; mpq_sub(r.val, val, o.val);
        return r;
    }

    Fraction operator*(const Fraction& o) const {
        Fraction r; mpq_mul(r.val, val, o.val);
        return r;
    }

    Fraction operator/(const Fraction& o) const {
        if (mpq_sgn(o.val) == 0) throw runtime_error("Деление на 0");
        Fraction r; mpq_div(r.val, val, o.val);
        return r;
    }

    // Операторы сравнения
    bool operator==(const Fraction& o) const { return mpq_equal(val, o.val); }
    bool operator!=(const Fraction& o) const { return !(*this == o); }
    bool operator>(const Fraction& o) const { return mpq_cmp(val, o.val) > 0; }

    friend ostream& operator<<(ostream& os, const Fraction& f) {
        char* s = mpq_get_str(NULL, 10, f.val);
        os << s;
        free(s);
        return os;
    }
};

#endif
