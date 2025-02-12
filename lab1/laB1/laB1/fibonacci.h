#ifndef FIBONACCI_H
#define FIBONACCI_H

#include <vector>
#include <stdexcept>


template <typename T>
std::vector<T> generateFibonacci(int n) {
    if (n <= 0) {
        throw std::invalid_argument("n must be a positive integer.");
    }
    std::vector<T> fibonacci;
    fibonacci.reserve(n);
    if (n >= 1) fibonacci.push_back(0);
    if (n >= 2) fibonacci.push_back(1);
    for (int i = 2; i < n; ++i) {
        T next = fibonacci[i - 1] + fibonacci[i - 2];
        fibonacci.push_back(next);
    }
    return fibonacci;
}

#endif // FIBONACCI_H
