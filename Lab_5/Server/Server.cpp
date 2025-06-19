#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <vector>

int main() {
    //  Создание анонимного канала
    HANDLE hReadPipe, hWritePipe;
    SECURITY_ATTRIBUTES sa = {
        sizeof(SECURITY_ATTRIBUTES),
        NULL,
        TRUE // Разрешаем наследование дескрипторов
    };

    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        std::cerr << "CreatePipe failed: " << GetLastError() << std::endl;
        return 1;
    }

    
    HANDLE hMutex = CreateMutexA(NULL, FALSE, "Global\\PipeMutex");
    if (!hMutex) {
        std::cerr << "CreateMutex failed: " << GetLastError() << std::endl;
        return 1;
    }

   
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    // Перенаправляем стандартные потоки Small
    si.hStdInput = hReadPipe;  // Small будет читать из этого канала
    si.hStdOutput = hWritePipe; // Small будет писать в этот канал
    si.dwFlags = STARTF_USESTDHANDLES;

    CHAR cmdLine[] = "Small.exe";

    if (!CreateProcessA(
        NULL, cmdLine,
        NULL, NULL,
        TRUE, // Наследование дескрипторов
        0, NULL, NULL,
        &si, &pi
    )) {
        std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
        return 1;
    }

    
    int size = 5;
    std::vector<int> arr = { -2, 5, 4, 8, 6 };
    int N = 100;
    /*
    int size, N;
    std::cout << "Enter array size: ";
    std::cin >> size;
    std::vector<int> arr(size);
    std::cout << "Enter " << size << " integers:\n";
    for (int i = 0; i < size; i++) {
        std::cin >> arr[i];
    }
    std::cout << "Enter N: ";
    std::cin >> N;
    */
   
    DWORD written;
    WaitForSingleObject(hMutex, INFINITE);

    WriteFile(hWritePipe, &size, sizeof(int), &written, NULL);
    WriteFile(hWritePipe, arr.data(), size * sizeof(int), &written, NULL);
    WriteFile(hWritePipe, &N, sizeof(int), &written, NULL);

    ReleaseMutex(hMutex);

    WaitForSingleObject(pi.hProcess, INFINITE);

    //  Чтение результата
    int count;
    std::vector<int> res;
    ReadFile(hReadPipe, &count, sizeof(int), &written, NULL);
    res.resize(count);
    ReadFile(hReadPipe, res.data(), count * sizeof(int), &written, NULL);

    
    std::cout << "\nReceived from Small:\nCount: " << count << "\nNumbers: ";
    for (int num : res) std::cout << num << " ";
    std::cout << std::endl;

    CloseHandle(hReadPipe);
    CloseHandle(hWritePipe);
    CloseHandle(hMutex);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}