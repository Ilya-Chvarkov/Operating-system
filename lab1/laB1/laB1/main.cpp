#include <iostream>
#include "fibonacci.h"

int main() {
    try {
        int n;
        std::cout << "Enter the number of Fibonacci numbers to generate: ";
        std::cin >> n;

        std::vector<int> fibonacci = generateFibonacci<int>(n);
        std::cout << "Fibonacci sequence: ";
        for (int num : fibonacci) {
            std::cout << num << " ";
        }
        std::cout << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
