#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <tchar.h>

#define MAX_READERS 3


const wchar_t* semaphoreName = L"ReaderSemaphore";
const wchar_t* eventAName = L"MessageAEvent";
const wchar_t* eventBName = L"MessageBEvent";
const wchar_t* eventCName = L"MessageCEvent";
const wchar_t* eventDName = L"MessageDEvent";
const wchar_t* endSessionEventName = L"EndSessionEvent";

int main() {
   
    HANDLE hSemaphore = CreateSemaphore(NULL, MAX_READERS, MAX_READERS, semaphoreName);
    HANDLE hEventA = CreateEvent(NULL, FALSE, FALSE, eventAName); 
    HANDLE hEventB = CreateEvent(NULL, FALSE, FALSE, eventBName); 
    HANDLE hEndSessionEvent = CreateEvent(NULL, TRUE, FALSE, endSessionEventName); // ручной

    if (hSemaphore == NULL || hEventA == NULL || hEventB == NULL || hEndSessionEvent == NULL) {
        std::cerr << "Failed to create synchronization objects. Error: " << GetLastError() << std::endl;
        return 1;
    }

    int M;
    std::cout << "Enter the number of Reader_Writer processes (M >= 3): ";
    std::cin >> M;
    if (M < 3) {
        std::cerr << "Number of processes must be at least 3." << std::endl;
        return 1;
    }

    
    int messageCount;
    std::cout << "Enter the number of messages to send: ";
    std::cin >> messageCount;

    std::vector<STARTUPINFO> si(M);
    std::vector<PROCESS_INFORMATION> pi(M);
    std::vector<std::wstring> commandLines(M);

    for (int i = 0; i < M; ++i) {
        commandLines[i] = L"Reader_Writer.exe";

        ZeroMemory(&si[i], sizeof(STARTUPINFO));
        si[i].cb = sizeof(STARTUPINFO);

        if (!CreateProcess(
            NULL,
            &commandLines[i][0],
            NULL,
            NULL,
            FALSE,
            CREATE_NEW_CONSOLE,
            NULL,
            NULL,
            &si[i],
            &pi[i])) {
            std::cerr << "Failed to create process " << i << ". Error: " << GetLastError() << std::endl;
            return 1;
        }
    }

    for (int i = 0; i < messageCount; ++i) {
        std::string message;
        std::cout << "Enter message (A or B): ";
        std::cin >> message;

        if (message == "A") {
            SetEvent(hEventA);
            std::cout << "Sent message A" << std::endl;
        }
        else if (message == "B") {
            SetEvent(hEventB);
            std::cout << "Sent message B" << std::endl;
        }
        else {
            std::cout << "Invalid message. Only A or B are allowed." << std::endl;
            --i; 
        }
    }

   
    SetEvent(hEndSessionEvent);
    std::cout << "Sent end session signal" << std::endl;

    WaitForMultipleObjects(M, &pi[0].hProcess, TRUE, INFINITE);

    for (int i = 0; i < M; ++i) {
        CloseHandle(pi[i].hProcess);
        CloseHandle(pi[i].hThread);
    }

    CloseHandle(hSemaphore);
    CloseHandle(hEventA);
    CloseHandle(hEventB);
    CloseHandle(hEndSessionEvent);

    return 0;
}