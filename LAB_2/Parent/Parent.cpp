#include <windows.h>
#include <iostream>
#include <string>
#include <vector>

int main() {
    system("chcp 1251"); 

    size_t n;
    std::cout << "Введите размер массива: ";
    std::cin >> n;

    std::vector<unsigned int> vec(n);
    std::cout << "Введите элементы массива (целые числа без знака): ";
    for (size_t i = 0; i < n; ++i) {
        std::cin >> vec[i];
    }

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USEPOSITION; 
    si.dwX = 200; 
    

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

    
  //  std::wstring appPath = L"D:\\4семестр\\Operating-system\\LAB_2\\Debug\\Child.exe";
    std::wstring appPath = L"Child.exe";
    std::wstring commandLine = appPath + L" " + std::to_wstring(n);
    for (const unsigned int num : vec) {
        commandLine += L" " + std::to_wstring(num); 
    }

    std::vector<wchar_t> commandLineBuffer(commandLine.begin(), commandLine.end());
    commandLineBuffer.push_back(L'\0'); 

    
    BOOL result = CreateProcessW(
        appPath.c_str(),
        commandLineBuffer.data(), 
        nullptr,
        nullptr,
        FALSE,
        CREATE_NEW_CONSOLE,
        nullptr,
        nullptr,
        &si,
        &pi
    );

    if (result) {
        std::cout << "Дочерний процесс успешно создан." << std::endl;

        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else {
        std::cout << "Не удалось создать дочерний процесс. Код ошибки: " << GetLastError() << std::endl;
    }

    return 0;
}