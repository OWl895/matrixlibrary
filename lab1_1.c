#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>

#ifdef _WIN32
#include <direct.h>
#define createDirectory(dir) _mkdir(dir)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define createDirectory(dir) mkdir(dir, 0777)
#endif

// Функция генерации случайного числа
double generateRandomNumber(double a, double b, int precision) {
    double num = a + (b - a) * ((double)rand() / RAND_MAX);
    double p = pow(10, precision);
    return round(num * p) / p;
}


// Перевод целой части числа в двоичную систему
void intToBinary(unsigned long long num, int* binary, int* length) {
    if (num == 0) {
        binary[0] = 0;
        *length = 1;
        return;
    }
    
    int temp[64];
    int i = 0;
    
    while (num > 0) {
        temp[i] = num % 2;  
        num = num / 2;       
        i++;
    }
    
    *length = i;
    for (int j = 0; j < i; j++) {
        binary[j] = temp[i - 1 - j];
    }
}

// Перевод дробной части числа в двоичную систему 
void fracToBinary(double frac, int* binary, int* length, int maxBits) {
    double f = frac;
    int i = 0;
    
    while (f > 0 && i < maxBits) {
        f = f * 2;
        if (f >= 1) {
            binary[i] = 1;
            f = f - 1;
        } else {
            binary[i] = 0;
        }
        i++;
    }
    *length = i;
}

// Восстановление числа из двоичного представления
unsigned long long binaryToInt(int* binary, int length) {
    unsigned long long result = 0;
    for (int i = 0; i < length; i++) {
        result = result * 2 + binary[i];
    }
    return result;
}


double binaryToFrac(int* binary, int length) {
    double result = 0.0;
    double power = 0.5;
    for (int i = 0; i < length; i++) {
        if (binary[i] == 1) {
            result += power;
        }
        power /= 2;
    }
    return result;
}

void splitNumber(double num, unsigned long long* intPart, double* fracPart) {
    *intPart = (unsigned long long)num;
    *fracPart = num - *intPart;
}

double convertByDivision(double original, int maxFracBits) {
    unsigned long long intPart;
    double fracPart;
    
    splitNumber(original, &intPart, &fracPart);
    
    int intBinary[64];
    int intLength;
    intToBinary(intPart, intBinary, &intLength);
    
    int fracBinary[64];
    int fracLength;
    fracToBinary(fracPart, fracBinary, &fracLength, maxFracBits);
    
    unsigned long long restoredInt = binaryToInt(intBinary, intLength);
    double restoredFrac = binaryToFrac(fracBinary, fracLength);
    
    double restored = (double)restoredInt + restoredFrac;
    
    return restored;
}

void writeFirstFile(const char* filename, double* numbers, int k, int precision) {
    FILE* f = fopen(filename, "w");

    fprintf(f, "| № | Число |\n");
    fprintf(f, "|---|-------|\n");

    for (int i = 0; i < k; i++) {
        fprintf(f, "| %d | %.*f |\n", i + 1, precision, numbers[i]);
    }

    fclose(f);
}

void writeSecondFile(const char* filename, double* numbers, int k, int precision, int bitness) {
    FILE* f = fopen(filename, "w");
    if (!f) {
        printf("Ошибка открытия %s\n", filename);
        return;
    }

    fprintf(f, "| № | Исходное число | Восстановленное число | Погрешность |\n");
    fprintf(f, "|---|----------------|----------------------|-------------|\n");

    for (int i = 0; i < k; i++) {
        double original = numbers[i];
        int maxFracBits;
        if (bitness == 32) {
            maxFracBits = 24; 
        } else if (bitness == 64) {
            maxFracBits = 53;  
        } else {
            maxFracBits = 64; 
        }
        
        double restored = convertByDivision(original, maxFracBits);
        double error = fabs(original - restored);
        
        fprintf(f, "| %d | %.*f | %.*f | %.2e |\n", 
                i + 1, precision, original, precision, restored, error);
    }

    fclose(f);
}

int main() {
    srand(time(NULL));

    FILE* inputFile = fopen("input.txt", "r");
    if (!inputFile) {
        printf("Ошибка открытия файла input.txt\n");
        return 1;
    }

    int n, k, bitness, precision;
    double a, b;
    
    int itemsRead = fscanf(inputFile, "%d %d %d %lf %lf %d", 
                          &n, &k, &bitness, &a, &b, &precision);
    fclose(inputFile);
    
    if (itemsRead != 6) {
        printf("Ошибка, %d параметров из 6\n", itemsRead);
        return 1;
    }
    
    
    createDirectory("Задания");
    createDirectory("Проверка");
    
    printf("Параметры программы:\n");
    printf("  Вариантов: %d\n", n);
    printf("  Чисел в варианте: %d\n", k);
    printf("  Разрядность: %d-bit\n", bitness);
    printf("  Диапазон: [%.2f, %.2f]\n", a, b);
    printf("  Точность вывода: %d знаков\n\n", precision);
   

    for (int variant = 1; variant <= n; variant++) {
        double* numbers = (double*)malloc(k * sizeof(double));
        if (!numbers) {
            printf("Ошибка выделения памяти для варианта %d\n", variant);
            return 1;
        }

        for (int i = 0; i < k; i++) {
            numbers[i] = generateRandomNumber(a, b, precision);
        }

        char filename1[100], filename2[100];

        sprintf(filename1, "Задания/var%02d.md", variant);
        sprintf(filename2, "Проверка/var%02d.md", variant);

        writeFirstFile(filename1, numbers, k, precision);
        writeSecondFile(filename2, numbers, k, precision, bitness);

        free(numbers);
        
        printf("Вариант %d обработан\n", variant);
    }

    printf("\nГенерация завершена!\n");
    printf("Созданы папки:\n");
    printf("  - Задания/ - исходные числа\n");
    printf("  - Проверка/ - результаты перевода (делением на 2)\n");

    return 0;
}
