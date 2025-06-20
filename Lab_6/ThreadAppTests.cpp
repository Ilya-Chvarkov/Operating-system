#include <gtest/gtest.h>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <semaphore>
#include <atomic>

// Объявляем функции и структуры, которые будем тестировать
extern "C" {
    struct ThreadArg {
        std::vector<int> array;
        int K;
        std::mutex startMutex;
        bool workStarted = false;
        std::mutex eventMutex;
        std::condition_variable eventCV;
        std::atomic<bool> workCompleted{false};
        std::binary_semaphore dataSem{1};
        std::vector<int> resultArray;
        int nonZeroCount = 0;
    };

    int countOnesInDecimal(int num);
    void Work(ThreadArg* threadArg);
    void CountElement(ThreadArg* threadArg);
}

// Тесты для функции countOnesInDecimal
TEST(CountOnesTest, BasicCases) {
    EXPECT_EQ(countOnesInDecimal(0), 0);
    EXPECT_EQ(countOnesInDecimal(1), 1);
    EXPECT_EQ(countOnesInDecimal(-1), 1);  // Проверяем отрицательные числа
    EXPECT_EQ(countOnesInDecimal(10), 1);
    EXPECT_EQ(countOnesInDecimal(11), 2);
    EXPECT_EQ(countOnesInDecimal(101), 2);
    EXPECT_EQ(countOnesInDecimal(111), 3);
    EXPECT_EQ(countOnesInDecimal(1001001), 4);
    EXPECT_EQ(countOnesInDecimal(-101), 2);  // Отрицательное число
}

// Тесты для функции Work
TEST(WorkThreadTest, FiltersNumbersCorrectly) {
    ThreadArg arg;
    arg.array = {1, -10, 11, 101, -111, 123, 1001, 0};
    arg.K = 2;
    arg.workStarted = true; // Пропускаем ожидание старта
    
    Work(&arg);

    std::vector<int> expected = {11, 101, -111};
    EXPECT_EQ(arg.resultArray, expected);
}

TEST(WorkThreadTest, HandlesEmptyArray) {
    ThreadArg arg;
    arg.array = {};
    arg.K = 1;
    arg.workStarted = true;
    
    Work(&arg);

    EXPECT_TRUE(arg.resultArray.empty());
}

// Тесты для функции CountElement
TEST(CountElementThreadTest, CountsNonZeroCorrectly) {
    ThreadArg arg;
    arg.array = {0, 1, 0, -2, 0, 3, 0, 4, 0};
    arg.workCompleted = true; // Пропускаем ожидание
    
    CountElement(&arg);

    EXPECT_EQ(arg.nonZeroCount, 4);
}

TEST(CountElementThreadTest, HandlesAllZeros) {
    ThreadArg arg;
    arg.array = {0, 0, 0, 0};
    arg.workCompleted = true;
    
    CountElement(&arg);

    EXPECT_EQ(arg.nonZeroCount, 0);
}

// Интеграционный тест
TEST(IntegrationTest, WorkAndCountElementTogether) {
    ThreadArg arg;
    arg.array = {0, 1, 11, 101, 0, -111, 123, 0, 1001};
    arg.K = 2;
    arg.workStarted = true;

    // Запускаем Work в основном потоке
    Work(&arg);

    // Проверяем результат Work
    std::vector<int> expectedResult = {11, 101, -111};
    EXPECT_EQ(arg.resultArray, expectedResult);

    // Теперь запускаем CountElement
    CountElement(&arg);
    EXPECT_EQ(arg.nonZeroCount, 6);
}

// Тест синхронизации
TEST(SynchronizationTest, MutexAndSemaphoreWork) {
    ThreadArg arg;
    arg.array = {1, 10, 11, 101, 111, 123, 1001};
    arg.K = 1;

    // Запускаем потоки
    std::thread workThread(Work, &arg);
    std::thread countThread(CountElement, &arg);

    // Даем сигнал на старт
    {
        std::lock_guard<std::mutex> lock(arg.startMutex);
        arg.workStarted = true;
    }
    arg.eventCV.notify_one();

    workThread.join();
    countThread.join();

    EXPECT_FALSE(arg.resultArray.empty());
    EXPECT_GT(arg.nonZeroCount, 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
