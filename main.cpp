#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include "matrix.h"

using namespace std;

// Вспомогательная функция для вывода вектора
void printVec(const string& label, const vector<Fraction>& v) {
    cout << label << " (Дроби): [ ";
    for(size_t i = 0; i < v.size(); ++i) cout << v[i] << (i == v.size() - 1 ? "" : ", ");
    cout << " ]" << endl;
    cout << label << " (Десятич): [ ";
    for(size_t i = 0; i < v.size(); ++i) cout << fixed << setprecision(4) << v[i].toDouble() << (i == v.size() - 1 ? "" : ", ");
    cout << " ]\n" << endl;
}

int main() {
    try {
        Matrix A(3, 3);
        A.data[0][0] = Fraction(1, 1); A.data[0][1] = Fraction(1, 1); A.data[0][2] = Fraction(1, 3);
        A.data[1][0] = Fraction(0, 1); A.data[1][1] = Fraction(2, 1); A.data[1][2] = Fraction(1, 5);
        A.data[2][0] = Fraction(0, 1); A.data[2][1] = Fraction(0, 1); A.data[2][2] = Fraction(3, 1);

        Matrix B(3, 3);
        for(int i=0; i<3; ++i) B.data[i][i] = Fraction(2); 

        cout << "МАТРИЦА A" << endl;
        A.printDouble();
        cout << "МАТРИЦА B" << endl;
        B.printDouble();

        cout << "\n1. Умножение A на вектор v=[1, 2, 3]:" << endl;
        vector<Fraction> v = {Fraction(1), Fraction(2), Fraction(3)};
        printVec("Результат", A.multiplyVector(v));

        cout << "2. Классическое умножение A * B:" << endl;
        Matrix::multiply(A, B).printDouble();

        cout << "\n3. Блочное умножение A * B:" << endl;
        Matrix::multiplyBlock(A, B, 2).printDouble();

        cout << "\n4. Алгоритм Штрассена A * B:" << endl;
        Matrix::multiplyStrassen(A, B).printDouble();

        vector<Fraction> b = {Fraction(5), Fraction(4), Fraction(3)};
        cout << "\nСЛАУ Ax = b, b=[5, 4, 3]" << endl;

        printVec("5. Метод Гаусса", Matrix::solveGauss(A, b));

        printVec("6. Метод Жордана-Гаусса", Matrix::solveJordanGauss(A, b));

        cout << "7. Обратная матрица (Метод Гаусса):" << endl;
        A.inverseGauss().printDouble();

        cout << "\n8. Обратная матрица (Метод Жордана-Гаусса):" << endl;
        A.inverseJordan().printDouble();

    } catch (const exception& e) {
        cerr << "\nОШИБКА: " << e.what() << endl;
    }
    return 0;
}
