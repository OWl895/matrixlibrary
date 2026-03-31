#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <iostream>
#include "fraction.h"

using namespace std;

class Matrix {
public:
    int rows, cols;
    vector<vector<Fraction>> data;

    Matrix(int r, int c);

    Matrix operator+(const Matrix& other) const;
    Matrix operator-(const Matrix& other) const;
    
    static Matrix identity(int n);
    void print() const;
    void printDouble() const;

    vector<Fraction> multiplyVector(const vector<Fraction>& v) const;
    static Matrix multiply(const Matrix& A, const Matrix& B);
    static Matrix multiplyBlock(const Matrix& A, const Matrix& B, int bSize = 2);
    static Matrix multiplyStrassen(const Matrix& A, const Matrix& B);

    static vector<Fraction> solveGauss(Matrix A, vector<Fraction> b);
    static vector<Fraction> solveJordanGauss(Matrix A, vector<Fraction> b);
    Matrix inverseJordan() const;
    Matrix inverseGauss() const;

private:
    static Matrix strassenRecursive(const Matrix& A, const Matrix& B);
    static int nextPowerOfTwo(int n);
};

#endif
