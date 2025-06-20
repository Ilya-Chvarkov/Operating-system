#include <gtest/gtest.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>

// Объявляем функции и структуры, которые будем тестировать
extern "C" {
    struct ThreadArg {
        std::vector<__int32> array;
        int K;
        HANDLE hMutex;
        HANDLE hEvent;
        CRITICAL_SECTION cs;
        std::vector<__int32> resultArray;
        int nonZeroCount;
    };

    DWORD WINAPI Work(LPVOID lpParam);
    DWORD WINAPI CountElement(LPVOID lpParam);
    int countOnesInDecimal(int num);
}

// Тесты для функции countOnesInDecimal
TEST(CountOnesTest, BasicCases) {
    EXPECT_EQ(countOnesInDecimal(0), 0);
    EXPECT_EQ(countOnesInDecimal(1), 1);
    EXPECT_EQ(countOnesInDecimal(10), 1);
    EXPECT_EQ(countOnesInDecimal(11), 2);
    EXPECT_EQ(countOnesInDecimal(101), 2);
    EXPECT_EQ(countOnesInDecimal(123), 1);
    EXPECT_EQ(countOnesInDecimal(111), 3);
    EXPECT_EQ(countOnesInDecimal(1001001), 4);
}

// Тесты для функции Work
TEST(WorkThreadTest, FiltersNumbersCorrectly) {
    ThreadArg arg;
    arg.array = {1, 10, 11, 101, 111, 123, 1001};
    arg.K = 2;
    arg.hMutex = CreateMutex(NULL, FALSE, NULL);
    arg.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    InitializeCriticalSection(&arg.cs);

    DWORD threadId;
    HANDLE hThread = CreateThread(NULL, 0, Work, &arg, 0, &threadId);
    WaitForSingleObject(hThread, INFINITE);

    std::vector<__int32> expected = {11, 101};
    EXPECT_EQ(arg.resultArray, expected);

    CloseHandle(hThread);
    CloseHandle(arg.hMutex);
    CloseHandle(arg.hEvent);
    DeleteCriticalSection(&arg.cs);
}

// Тесты для функции CountElement
TEST(CountElementThreadTest, CountsNonZeroCorrectly) {
    ThreadArg arg;
    arg.array = {0, 1, 0, 2, 0, 3, 0, 4, 0};
    arg.hMutex = CreateMutex(NULL, FALSE, NULL);
    arg.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    InitializeCriticalSection(&arg.cs);
    SetEvent(arg.hEvent); // Симулируем завершение Work потока

    DWORD threadId;
    HANDLE hThread = CreateThread(NULL, 0, CountElement, &arg, 0, &threadId);
    WaitForSingleObject(hThread, INFINITE);

    EXPECT_EQ(arg.nonZeroCount, 4);

    CloseHandle(hThread);
    CloseHandle(arg.hMutex);
    CloseHandle(arg.hEvent);
    DeleteCriticalSection(&arg.cs);
}

// Тест интеграции Work и CountElement
TEST(IntegrationTest, WorkAndCountElementTogether) {
    ThreadArg arg;
    arg.array = {0, 1, 11, 101, 0, 111, 123, 0, 1001};
    arg.K = 2;
    arg.hMutex = CreateMutex(NULL, FALSE, NULL);
    arg.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    InitializeCriticalSection(&arg.cs);

    DWORD workId, countId;
    HANDLE hWork = CreateThread(NULL, 0, Work, &arg, 0, &workId);
    HANDLE hCount = CreateThread(NULL, 0, CountElement, &arg, 0, &countId);

    WaitForSingleObject(hWork, INFINITE);
    WaitForSingleObject(hCount, INFINITE);

    std::vector<__int32> expectedResult = {11, 101};
    EXPECT_EQ(arg.resultArray, expectedResult);
    EXPECT_EQ(arg.nonZeroCount, 6);

    CloseHandle(hWork);
    CloseHandle(hCount);
    CloseHandle(arg.hMutex);
    CloseHandle(arg.hEvent);
    DeleteCriticalSection(&arg.cs);
}

// Тест для проверки синхронизации
TEST(SynchronizationTest, MutexAndCriticalSectionWork) {
    ThreadArg arg;
    arg.array = {1, 10, 11, 101, 111, 123, 1001};
    arg.K = 1;
    arg.hMutex = CreateMutex(NULL, TRUE, NULL); // Блокируем сразу
    arg.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    InitializeCriticalSection(&arg.cs);

    DWORD workId, countId;
    HANDLE hWork = CreateThread(NULL, 0, Work, &arg, 0, &workId);
    HANDLE hCount = CreateThread(NULL, 0, CountElement, &arg, 0, &countId);

    // Потоки должны ждать, пока мы не освободим мьютекс
    Sleep(100); // Даем время потокам начать ожидание
    ReleaseMutex(arg.hMutex);
    SetEvent(arg.hEvent);

    WaitForSingleObject(hWork, INFINITE);
    WaitForSingleObject(hCount, INFINITE);

    EXPECT_FALSE(arg.resultArray.empty());
    EXPECT_GT(arg.nonZeroCount, 0);

    CloseHandle(hWork);
    CloseHandle(hCount);
    CloseHandle(arg.hMutex);
    CloseHandle(arg.hEvent);
    DeleteCriticalSection(&arg.cs);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
