#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

bool isPrime(unsigned int num) {
    if (num < 2) return false;
    for (unsigned int i = 2; i <= std::sqrt(num); ++i) {
        if (num % i == 0) return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    system("chcp 1251"); 

    //std::cout << "Аргументы командной строки:" << std::endl;
    //for (int i = 0; i < argc; ++i) {
    //    std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
    //}

    if (argc < 2) {
        std::cerr << "Ошибка: недостаточно аргументов." << std::endl;
        system("pause");
        return 1;
    }

    size_t n;
    try {
        n = std::stoull(argv[1]);
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: неверный формат размера массива." << std::endl;
        system("pause");
        return 1;
    }

    std::vector<unsigned int> inputVec;

    for (int i = 2; i < argc; ++i) {
        try {
            inputVec.push_back(static_cast<unsigned int>(std::stoull(argv[i])));
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка: неверный формат элемента массива." << std::endl;
            system("pause");
            return 1;
        }
    }

    if (inputVec.size() != n) {
        std::cerr << "Ошибка: количество элементов не соответствует размеру массива." << std::endl;
        system("pause");
        return 1;
    }

   
    std::vector<unsigned int> resultVec(n, 0); 
    size_t resultIndex = 0;

    for (const unsigned int num : inputVec) {
        if (isPrime(num)) { 
            resultVec[resultIndex++] = num;
        }
    }

    
    std::cout << "Результат: ";
    for (const unsigned int num : resultVec) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    system("pause"); 
    return 0;
}