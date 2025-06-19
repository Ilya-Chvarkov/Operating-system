#include <Windows.h>
#include <iostream>
#include <vector>


struct ThreadArg {
    std::vector<__int32> array;
    int K;
    HANDLE hMutex;
    HANDLE hEvent;
    CRITICAL_SECTION cs;
    std::vector<__int32> resultArray;
    int nonZeroCount;
};

int countOnesInDecimal(int num) {
    int count = 0;
    while (num != 0) {
        if (num % 10 == 1) { 
            count++;
        }
        num /= 10; 
    }
    return count;
}

DWORD WINAPI Work(LPVOID lpParam) {
    ThreadArg* threadArg = (ThreadArg*)lpParam;

    WaitForSingleObject(threadArg->hMutex, INFINITE);

    std::cout << "Work thread is starting...\n";

    for (int num : threadArg->array) {
        if (countOnesInDecimal(num) == threadArg->K) {
            threadArg->resultArray.push_back(num);
        }
    }

    // потоку main и CountElement о работе
    SetEvent(threadArg->hEvent);

    std::cout << "Work thread is ending...\n";

    // Освобождаем
    ReleaseMutex(threadArg->hMutex);

    return 0;
}


DWORD WINAPI CountElement(LPVOID lpParam) {
    ThreadArg* threadArg = (ThreadArg*)lpParam;
    EnterCriticalSection(&threadArg->cs);
    
    WaitForSingleObject(threadArg->hEvent, INFINITE);

    std::cout << "CountElement thread is starting...\n";

    threadArg->nonZeroCount = 0;
    for (int num : threadArg->array) {
        if (num != 0) {
            threadArg->nonZeroCount++;
        }
    }

    // Сигнализируем потоку main о выводе результата

    std::cout << "CountElement thread is ending...\n";
    LeaveCriticalSection(&threadArg->cs);

    return 0;
}

int main() {
    ThreadArg threadArg;

    threadArg.hMutex = CreateMutex(NULL, TRUE, NULL);
    threadArg.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    InitializeCriticalSection(&threadArg.cs);

    int size;
    std::cout << "Enter array size: ";
    std::cin >> size;

    threadArg.array.resize(size);
    std::cout << "Enter array elements:\n";
    for (int i = 0; i < size; ++i) {
        std::cout << "Element " << i + 1 << ": ";
        std::cin >> threadArg.array[i];
    }

    std::cout << "Array: ";
    for (int num : threadArg.array) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    

    HANDLE hWork = CreateThread(NULL, 0, Work, &threadArg, 0, NULL);
    HANDLE hCountElement = CreateThread(NULL, 0, CountElement, &threadArg, 0, NULL);
    std::cout << "Enter K: ";
    std::cin >> threadArg.K;
    ReleaseMutex(threadArg.hMutex);

    // Ждем сигнала от потока work о выводе K элементов массива
    WaitForSingleObject(threadArg.hEvent, INFINITE);

    
    std::cout << "Result array (numbers with " << threadArg.K << " ones in decimal): ";
    for (int num : threadArg.resultArray) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    EnterCriticalSection(&threadArg.cs);
    std::cout << "Non-zero count: " << threadArg.nonZeroCount << std::endl;
    LeaveCriticalSection(&threadArg.cs);

    WaitForSingleObject(hWork, INFINITE);
    WaitForSingleObject(hCountElement, INFINITE);

    
    CloseHandle(hWork);
    CloseHandle(hCountElement);
    CloseHandle(threadArg.hMutex);
    CloseHandle(threadArg.hEvent);
    DeleteCriticalSection(&threadArg.cs);

    return 0;
}