#include "pch.h"

#include "fibonacci.h"
#include <gtest/gtest.h>

TEST(FibonacciTest, GeneratesCorrectSequence) {
    std::vector<int> expected = { 0, 1, 1, 2, 3, 5, 8, 13, 21, 34 };
    EXPECT_EQ(generateFibonacci<int>(10), expected);
}

TEST(FibonacciTest, BoundaryValues) {
    std::vector<int> expected1 = { 0 };
    std::vector<int> expected2 = { 0, 1 };
    EXPECT_EQ(generateFibonacci<int>(1), expected1);
    EXPECT_EQ(generateFibonacci<int>(2), expected2);
}

TEST(FibonacciTest, ThrowsExceptionForInvalidInput) {
    EXPECT_THROW(generateFibonacci<int>(-1), std::invalid_argument);
    EXPECT_THROW(generateFibonacci<int>(0), std::invalid_argument);
}

TEST(FibonacciTest, GeneratesCorrectSequenceWithLong) {
    std::vector<long> expected = { 0, 1, 1, 2, 3, 5, 8, 13, 21, 34 };
    EXPECT_EQ(generateFibonacci<long>(10), expected);
}

TEST(FibonacciTest, GeneratesCorrectSequenceWithDouble) {
    std::vector<double> expected = { 0.0, 1.0, 1.0, 2.0, 3.0, 5.0, 8.0, 13.0, 21.0, 34.0 };
    EXPECT_EQ(generateFibonacci<double>(10), expected);
}

TEST(FibonacciTest, GeneratesLargeSequence) {
    std::vector<long long> fibonacci = generateFibonacci<long long>(50);
    ASSERT_EQ(fibonacci.size(), 50);
    EXPECT_EQ(fibonacci[0], 0);
    EXPECT_EQ(fibonacci[1], 1);
    EXPECT_EQ(fibonacci[49], 7778742049LL); 
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
