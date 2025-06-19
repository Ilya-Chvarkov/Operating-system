#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <vector>

void PrintLastError(const char* context) {
    DWORD error = GetLastError();
    std::cerr << "ERROR in " << context << ": " << error << std::endl;
}

int main() {
    
    HANDLE hMutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, "Global\\PipeMutex");
    if (!hMutex) {
        PrintLastError("OpenMutex");
        return 1;
    }

    // получение дескр
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    int size = 0, N = 0;
    DWORD read;
    WaitForSingleObject(hMutex, INFINITE);

    if (!ReadFile(hInput, &size, sizeof(int), &read, NULL) || read != sizeof(int)) {
        PrintLastError("Read size");
    }

    std::vector<int> arr(size);
    if (!ReadFile(hInput, arr.data(), size * sizeof(int), &read, NULL) || read != size * sizeof(int)) {
        PrintLastError("Read array");
    }

    if (!ReadFile(hInput, &N, sizeof(int), &read, NULL) || read != sizeof(int)) {
        PrintLastError("Read N");
    }

    ReleaseMutex(hMutex);

    std::vector<int> negatives;
    for (int num : arr) {
        if (num < 0) negatives.push_back(num);
    }

    // отправка результата
    DWORD written;
    int count = negatives.size();
    if (!WriteFile(hOutput, &count, sizeof(int), &written, NULL)) {
        PrintLastError("Write count");
    }
    FlushFileBuffers(hOutput);

    if (count > 0) {
        if (!WriteFile(hOutput, negatives.data(), count * sizeof(int), &written, NULL)) {
            PrintLastError("Write negatives");
        }
        FlushFileBuffers(hOutput);
    }

    std::cerr << "Small: processed " << size << " elements. Found " << count << " negatives.\n";

    CloseHandle(hInput);
    CloseHandle(hOutput);
    CloseHandle(hMutex);

    return 0;
}