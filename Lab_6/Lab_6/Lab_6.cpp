//#include <iostream>
//#include <vector>
//#include <thread>
//#include <mutex>
//#include <condition_variable>
//#include <semaphore> 
//
//int countOnesInDecimal(int num) {
//    int count = 0;
//    num = abs(num);
//    while (num != 0) {
//        if (num % 10 == 1) {
//            count++;
//        }
//        num /= 10;
//    }
//    return count;
//}
//
//struct ThreadArg {
//    std::vector<int> array;
//    int K;
//    std::mutex mutex;
//    std::condition_variable_any cv;
//    std::binary_semaphore dataSem; 
//    std::vector<int> resultArray;
//    int nonZeroCount = 0;
//    bool workDone = false;
//
//    ThreadArg() : dataSem(1) {}//констр
//};
//
//void Work(ThreadArg* threadArg) {
//    std::unique_lock<std::mutex> lock(threadArg->mutex);
//
//    for (int num : threadArg->array) {
//        if (countOnesInDecimal(num) == threadArg->K) {
//            threadArg->resultArray.push_back(num);
//        }
//    }
//
//    threadArg->workDone = true;
//    lock.unlock();
//    threadArg->cv.notify_all();
//}
//
//void CountElement(ThreadArg* threadArg) {
//    threadArg->dataSem.acquire();
//    std::unique_lock<std::mutex> lock(threadArg->mutex);
//    threadArg->cv.wait(lock, [&] { return threadArg->workDone; });
//
//   
//    for (int num : threadArg->array) {
//        if (num != 0) {
//            threadArg->nonZeroCount++;
//        }
//    }
//    threadArg->dataSem.release();
//}
//
//int main() {
//    ThreadArg threadArg;
//
//    int size;
//    std::cout << "Enter array size: ";
//    std::cin >> size;
//
//    threadArg.array.resize(size);
//    std::cout << "Enter array elements:\n";
//    for (int i = 0; i < size; ++i) {
//        std::cout << "Element " << i + 1 << ": ";
//        std::cin >> threadArg.array[i];
//    }
//
//    std::unique_lock<std::mutex> mainLock(threadArg.mutex);
//
//    std::thread workThread(Work, &threadArg);
//    std::thread countThread(CountElement, &threadArg);
//
//    std::cout << "Enter K: ";
//    std::cin >> threadArg.K;
//    mainLock.unlock();
//
//
//    std::cout << "Result array (numbers with " << threadArg.K << " ones in decimal): ";
//    for (int num : threadArg.resultArray) {
//        std::cout << num << " ";
//    }
//
//
//    threadArg.dataSem.acquire();
//    std::cout << "Non-zero count: " << threadArg.nonZeroCount << std::endl;
//    threadArg.dataSem.release();
//
//  
//    std::cout << std::endl;
//    workThread.join();
//    countThread.join();
//    return 0;
//}
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <semaphore>
#include <atomic>

int countOnesInDecimal(int num) {
    int count = 0;
    num = abs(num);
    while (num != 0) {
        if (num % 10 == 1) {
            count++;
        }
        num /= 10;
    }
    return count;
}

struct ThreadArg {
    std::vector<int> array;
    int K;

    std::mutex startMutex;
    bool workStarted = false;

    std::mutex eventMutex;
    std::condition_variable eventCV;
    std::atomic<bool> workCompleted{ false };

    
    std::binary_semaphore dataSem{ 1 };

    std::vector<int> resultArray;
    int nonZeroCount = 0;
};

void Work(ThreadArg* threadArg) {
    
    {
        std::unique_lock<std::mutex> lock(threadArg->startMutex);
        threadArg->eventCV.wait(lock, [&] { return threadArg->workStarted; });
    }

    threadArg->dataSem.acquire();
    for (int num : threadArg->array) {
        if (countOnesInDecimal(num) == threadArg->K) {
            threadArg->resultArray.push_back(num);
        }
    }
    threadArg->dataSem.release();

    
    {
        std::lock_guard<std::mutex> lock(threadArg->eventMutex);
        threadArg->workCompleted = true;
    }
    threadArg->eventCV.notify_all();
}

void CountElement(ThreadArg* threadArg) {
    
    {
        std::unique_lock<std::mutex> lock(threadArg->eventMutex);
        threadArg->eventCV.wait(lock, [&] { return threadArg->workCompleted.load(); });
    }

   
    threadArg->dataSem.acquire();
    for (int num : threadArg->array) {
        if (num != 0) {
            threadArg->nonZeroCount++;
        }
    }
    threadArg->dataSem.release();
}

int main() {
    ThreadArg threadArg;

    int size;
    std::cout << "Enter array size: ";
    std::cin >> size;

    threadArg.array.resize(size);
    std::cout << "Enter array elements:\n";
    for (int i = 0; i < size; ++i) {
        std::cout << "Element " << i + 1 << ": ";
        std::cin >> threadArg.array[i];
    }

    std::thread workThread(Work, &threadArg);
    std::thread countThread(CountElement, &threadArg);

    std::cout << "Enter K: ";
    std::cin >> threadArg.K;

    {
        std::lock_guard<std::mutex> lock(threadArg.startMutex);
        threadArg.workStarted = true;
    }
    threadArg.eventCV.notify_one();

    {
        std::unique_lock<std::mutex> lock(threadArg.eventMutex);
        threadArg.eventCV.wait(lock, [&] { return threadArg.workCompleted.load(); });
    }

    threadArg.dataSem.acquire();
    std::cout << "Result array (numbers with " << threadArg.K << " ones in decimal): ";
    for (int num : threadArg.resultArray) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
    threadArg.dataSem.release();

    threadArg.dataSem.acquire();
    std::cout << "Non-zero count: " << threadArg.nonZeroCount << std::endl;
    threadArg.dataSem.release();

    workThread.join();
    countThread.join();

    return 0;
}