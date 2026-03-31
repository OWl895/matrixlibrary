#include "matrix.h"
#include <algorithm>
#include <iomanip>

using namespace std;

Matrix::Matrix(int r, int c) : rows(r), cols(c), data(r, vector<Fraction>(c)) {}

int Matrix::nextPowerOfTwo(int n) {
    int m = 1;
    while (n > m) {
        m *= 2;
    }
    return m;
}

Matrix Matrix::operator+(const Matrix& other) const {
    Matrix res(rows, cols);
    for(int i=0; i != rows; ++i)
        for(int j=0; j != cols; ++j) res.data[i][j] = data[i][j] + other.data[i][j];
    return res;
}

Matrix Matrix::operator-(const Matrix& other) const {
    Matrix res(rows, cols);
    for(int i=0; i != rows; ++i)
        for(int j=0; j != cols; ++j) res.data[i][j] = data[i][j] - other.data[i][j];
    return res;
}

void Matrix::print() const {
    for(int i = 0; i != rows; ++i) {
        for(int j = 0; j != cols; ++j) cout << data[i][j] << "\t";
        cout << endl;
    }
}

void Matrix::printDouble() const {
    for(int i = 0; i != rows; ++i) {
        for(int j = 0; j != cols; ++j) {
            cout << fixed << setprecision(4) << data[i][j].toDouble() << "\t";
        }
        cout << endl;
    }
}

vector<Fraction> Matrix::multiplyVector(const vector<Fraction>& v) const {
    vector<Fraction> res(rows);
    for(int i = 0; i != rows; ++i)
        for(int j = 0; j != cols; ++j) res[i] = res[i] + (data[i][j] * v[j]);
    return res;
}

Matrix Matrix::multiply(const Matrix& A, const Matrix& B) {
    Matrix res(A.rows, B.cols);
    for(int i = 0; i != A.rows; ++i)
        for(int k = 0; k != A.cols; ++k)
            for(int j = 0; j != B.cols; ++j)
                res.data[i][j] = res.data[i][j] + (A.data[i][k] * B.data[k][j]);
    return res;
}

Matrix Matrix::multiplyBlock(const Matrix& A, const Matrix& B, int bSize) {
    Matrix res(A.rows, B.cols);
    for (int i0 = 0; i0 < A.rows; i0 += bSize)
        for (int j0 = 0; j0 < B.cols; j0 += bSize)
            for (int k0 = 0; k0 < A.cols; k0 += bSize)
                for (int i = i0; i < min(i0 + bSize, A.rows); ++i)
                    for (int j = j0; j < min(j0 + bSize, B.cols); ++j)
                        for (int k = k0; k < min(k0 + bSize, A.cols); ++k)
                            res.data[i][j] = res.data[i][j] + (A.data[i][k] * B.data[k][j]);
    return res;
}

Matrix Matrix::multiplyStrassen(const Matrix& A, const Matrix& B) {
    int maxSize = max({A.rows, A.cols, B.cols});
    int m = nextPowerOfTwo(maxSize);
    Matrix Ap(m, m), Bp(m, m);
    for(int i=0; i != A.rows; ++i) for(int j=0; j != A.cols; ++j) Ap.data[i][j] = A.data[i][j];
    for(int i=0; i != B.rows; ++i) for(int j=0; j != B.cols; ++j) Bp.data[i][j] = B.data[i][j];
    Matrix Resp = strassenRecursive(Ap, Bp);
    Matrix res(A.rows, B.cols);
    for(int i=0; i != A.rows; ++i) for(int j=0; j != B.cols; ++j) res.data[i][j] = Resp.data[i][j];
    return res;
}

Matrix Matrix::strassenRecursive(const Matrix& A, const Matrix& B) {
    int n = A.rows;
    if (n <= 2) return multiply(A, B);
    int m = n / 2;
    Matrix a11(m,m), a12(m,m), a21(m,m), a22(m,m), b11(m,m), b12(m,m), b21(m,m), b22(m,m);
    for(int i=0; i != m; i++) {
        for(int j=0; j != m; j++) {
            a11.data[i][j]=A.data[i][j]; a12.data[i][j]=A.data[i][j+m];
            a21.data[i][j]=A.data[i+m][j]; a22.data[i][j]=A.data[i+m][j+m];
            b11.data[i][j]=B.data[i][j]; b12.data[i][j]=B.data[i][j+m];
            b21.data[i][j]=B.data[i+m][j]; b22.data[i][j]=B.data[i+m][j+m];
        }
    }
    Matrix p1 = strassenRecursive(a11+a22, b11+b22);
    Matrix p2 = strassenRecursive(a21+a22, b11);
    Matrix p3 = strassenRecursive(a11, b12-b22);
    Matrix p4 = strassenRecursive(a22, b21-b11);
    Matrix p5 = strassenRecursive(a11+a12, b22);
    Matrix p6 = strassenRecursive(a21-a11, b11+b12);
    Matrix p7 = strassenRecursive(a12-a22, b21+b22);
    Matrix res(n, n);
    for(int i=0; i != m; i++) {
        for(int j=0; j != m; j++) {
            res.data[i][j] = p1.data[i][j]+p4.data[i][j]-p5.data[i][j]+p7.data[i][j];
            res.data[i][j+m] = p3.data[i][j]+p5.data[i][j];
            res.data[i+m][j] = p2.data[i][j]+p4.data[i][j];
            res.data[i+m][j+m] = p1.data[i][j]-p2.data[i][j]+p3.data[i][j]+p6.data[i][j];
        }
    }
    return res;
}

vector<Fraction> Matrix::solveGauss(Matrix A, vector<Fraction> b) {
    int n = A.rows;
    for(int i = 0; i != n; i++) {
        int pivot = i;
        for(int j = i + 1; j < n; j++) if(A.data[j][i] > A.data[pivot][i]) pivot = j;
        swap(A.data[i], A.data[pivot]); swap(b[i], b[pivot]);
        for(int j = i + 1; j < n; j++) {
            Fraction f = A.data[j][i] / A.data[i][i];
            for(int k = i; k < n; k++) A.data[j][k] = A.data[j][k] - (f * A.data[i][k]);
            b[j] = b[j] - (f * b[i]);
        }
    }
    vector<Fraction> x(n);
    for(int i = n - 1; i >= 0; i--) {
        Fraction s(0);
        for(int j = i + 1; j < n; j++) s = s + (A.data[i][j] * x[j]);
        x[i] = (b[i] - s) / A.data[i][i];
    }
    return x;
}

vector<Fraction> Matrix::solveJordanGauss(Matrix A, vector<Fraction> b) {
    int n = A.rows;
    for (int i = 0; i != n; i++) {
        Fraction div = A.data[i][i];
        for (int j = i; j != n; j++) A.data[i][j] = A.data[i][j] / div;
        b[i] = b[i] / div;
        for (int k = 0; k != n; k++) {
            if (k != i) {
                Fraction f = A.data[k][i];
                for (int j = i; j != n; j++) A.data[k][j] = A.data[k][j] - (f * A.data[i][j]);
                b[k] = b[k] - (f * b[i]);
            }
        }
    }
    return b;
}

Matrix Matrix::inverseJordan() const {
    int n = rows;
    Matrix aug(n, 2 * n);
    for(int i = 0; i != n; i++) {
        for(int j = 0; j != n; j++) aug.data[i][j] = data[i][j];
        aug.data[i][i + n] = Fraction(1);
    }
    for(int i = 0; i != n; i++) {
        Fraction div = aug.data[i][i];
        for(int j = 0; j < 2 * n; j++) aug.data[i][j] = aug.data[i][j] / div;
        for(int k = 0; k != n; k++) {
            if(k != i) {
                Fraction f = aug.data[k][i];
                for(int j = 0; j < 2 * n; j++) aug.data[k][j] = aug.data[k][j] - (f * aug.data[i][j]);
            }
        }
    }
    Matrix res(n, n);
    for(int i = 0; i != n; i++) for(int j = 0; j != n; j++) res.data[i][j] = aug.data[i][j + n];
    return res;
}

Matrix Matrix::inverseGauss() const {
    int n = rows;
    Matrix aug(n, 2 * n);
    for(int i = 0; i != n; i++) {
        for(int j = 0; j != n; j++) aug.data[i][j] = data[i][j];
        aug.data[i][i + n] = Fraction(1);
    }
    for(int i = 0; i != n; i++) {
        for(int j = i + 1; j < n; j++) {
            Fraction f = aug.data[j][i] / aug.data[i][i];
            for(int k = i; k < 2 * n; k++) aug.data[j][k] = aug.data[j][k] - (f * aug.data[i][k]);
        }
    }
    for(int i = n - 1; i >= 0; i--) {
        Fraction div = aug.data[i][i];
        for(int j = i; j < 2 * n; j++) aug.data[i][j] = aug.data[i][j] / div;
        for(int k = i - 1; k >= 0; k--) {
            Fraction f = aug.data[k][i];
            for(int j = i; j < 2 * n; j++) aug.data[k][j] = aug.data[k][j] - (f * aug.data[i][j]);
        }
    }
    Matrix res(n, n);
    for(int i = 0; i != n; i++) for(int j = 0; j != n; j++) res.data[i][j] = aug.data[i][j + n];
    return res;
}

Matrix Matrix::identity(int n) {
    Matrix res(n, n);
    for(int i = 0; i != n; ++i) res.data[i][i] = Fraction(1);
    return res;
}
