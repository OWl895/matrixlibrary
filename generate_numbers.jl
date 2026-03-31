using Primes
using Random

# Функция для генерации случайного числа заданной длины
function generate_random_number(length)
    startt = BigInt(10)^(length-1)
    endd = BigInt(10)^length - 1
    return rand(startt:endd)
end

# Функция для генерации простого числа
function generate_prime(length)
    while true
        n = generate_random_number(length)
        if isprime(n)
            return n
        end
    end
end

# Функция для генерации составного числа
function generate_composite(length)
    half_len = div(length, 2) + 1
    n1 = generate_random_number(half_len)
    n2 = generate_random_number(half_len)
    n1 = max(n1, 2)
    n2 = max(n2, 2)
    return n1 * n2
end

kn = 200   # минимальная длина числа
kk = 1000  # максимальная длина числа
z = 30     # количество чисел

println("Диапазон длины чисел: от $kn до $kk")
println("Всего чисел: $z\n")

open("numbers.txt", "w") do file
    # Генерируем z/2 простых чисел
    prime_count = div(z, 2)
    
    for i in 1:prime_count
        p = rand(kn:kk)  # случайная длина числа
        prime_num = generate_prime(p)
        println(file, "prime $prime_num")
        println("Простое число #$i (длина $p цифр)")
    end
    
    # Генерируем z/2 составных чисел
    composite_count = z - prime_count
    
    for i in 1:composite_count
        p = rand(kn:kk)  # случайная длина числа
        composite_num = generate_composite(p)
        println(file, "composite $composite_num")
        println("Составное число #$i (длина $p цифр)")
    end
end

