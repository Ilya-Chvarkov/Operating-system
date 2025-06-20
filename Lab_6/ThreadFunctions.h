// ThreadFunctions.h
#pragma once
#include <vector>
#include <mutex>
#include <condition_variable>
#include <semaphore>
#include <atomic>

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
