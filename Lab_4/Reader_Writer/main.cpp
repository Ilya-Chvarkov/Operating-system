#include <windows.h>
#include <iostream>
#include <string>
#include <thread>

#define MAX_READERS 3

const wchar_t* semaphoreName = L"ReaderSemaphore";
const wchar_t* eventAName = L"MessageAEvent";
const wchar_t* eventBName = L"MessageBEvent";
const wchar_t* eventCName = L"MessageCEvent";
const wchar_t* eventDName = L"MessageDEvent";
const wchar_t* endSessionEventName = L"EndSessionEvent";
const wchar_t* consoleMutexName = L"ConsoleMutex";

HANDLE hEvents[4];
bool stopThread = false;

DWORD WINAPI MessageReceiver(LPVOID) {
    HANDLE hEndEvent = OpenEvent(SYNCHRONIZE, FALSE, endSessionEventName);
    HANDLE hMutex = CreateMutex(NULL, FALSE, consoleMutexName);

    while (!stopThread) {
        DWORD result = WaitForMultipleObjects(4, hEvents, FALSE, 3);

        if (result >= WAIT_OBJECT_0 && result <= WAIT_OBJECT_0 + 3) {
            WaitForSingleObject(hMutex, INFINITE);
            std::cout << "\nReceived message: "
                << (char)('A' + (result - WAIT_OBJECT_0))
                << " (Activated)\n";
            ReleaseMutex(hMutex);
        }

        if (WaitForSingleObject(hEndEvent, 0) == WAIT_OBJECT_0)
            stopThread = true;
    }

    CloseHandle(hEndEvent);
    CloseHandle(hMutex);
    return 0;
}

int main() {
    
    HANDLE hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, semaphoreName);
    HANDLE hEventA = OpenEvent(EVENT_MODIFY_STATE | SYNCHRONIZE, FALSE, eventAName);
    HANDLE hEventB = OpenEvent(EVENT_MODIFY_STATE | SYNCHRONIZE, FALSE, eventBName);
    HANDLE hEventC = CreateEvent(NULL, FALSE, FALSE, eventCName);
    HANDLE hEventD = CreateEvent(NULL, FALSE, FALSE, eventDName);
    HANDLE hEndEvent = OpenEvent(SYNCHRONIZE, FALSE, endSessionEventName);
    HANDLE hConsoleMutex = CreateMutex(NULL, FALSE, consoleMutexName);

    hEvents[0] = hEventA;
    hEvents[1] = hEventB;
    hEvents[2] = hEventC;
    hEvents[3] = hEventD;

    
    WaitForSingleObject(hSemaphore, INFINITE);
    WaitForSingleObject(hConsoleMutex, INFINITE);
    std::cout << "Semaphore captured (Active)\n";
    ReleaseMutex(hConsoleMutex);
    
    HANDLE hThread = CreateThread(NULL, 0, MessageReceiver, NULL, 0, NULL);

    while (true) {
        std::string msg;
        WaitForSingleObject(hConsoleMutex, INFINITE);
        std::cout << "Enter message (C/D) or 'exit': ";
        ReleaseMutex(hConsoleMutex);

        std::cin >> msg;

        if (msg == "exit") break;

        if (msg == "C" || msg == "D") {
            HANDLE hEvent = (msg == "C") ? hEventC : hEventD;
            SetEvent(hEvent);
            WaitForSingleObject(hConsoleMutex, INFINITE);
            std::cout << "\nSent message " << msg << std::endl;
            ReleaseMutex(hConsoleMutex);
        }
    }

    
    stopThread = true;
    WaitForSingleObject(hThread, INFINITE);

    ReleaseSemaphore(hSemaphore, 1, NULL);
    WaitForSingleObject(hConsoleMutex, INFINITE);
    std::cout << "\nSemaphore released (Inactive)\n";
    ReleaseMutex(hConsoleMutex);

    CloseHandle(hSemaphore);
    CloseHandle(hEventA);
    CloseHandle(hEventB);
    CloseHandle(hEventC);
    CloseHandle(hEventD);
    CloseHandle(hEndEvent);
    CloseHandle(hConsoleMutex);
    CloseHandle(hThread);

    return 0;
}