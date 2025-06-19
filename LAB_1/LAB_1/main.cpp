#include <iostream>
#include <windows.h>
using namespace std;


struct structure {
    char* str;  
    int size;   
};


DWORD WINAPI workerFunction(LPVOID lpParam) {
    structure* s = (structure*)lpParam;
    cout << "Nolatin: ";

    for (int i = 0; i < s->size; i++) {
        
        if ((unsigned char)s->str[i] >= 128) {
            cout << s->str[i] << " ";
        }
    }

    cout << endl;
    return 0;
}

int main() {
    
   
    int size;
    cout << "Size: ";
    cin >> size;
    cin.ignore();  

    char* str = new char[size + 1];
    cout << "stroka: ";
    cin.getline(str, size + 1);

    int time;
    cout << "time for sleep(ms): ";
    cin >> time;

    structure* s = new structure;
    s->str = str;
    s->size = size;

    HANDLE worker;
    DWORD IDThread;
    worker = CreateThread(NULL, 0, workerFunction, (LPVOID)s, CREATE_SUSPENDED, &IDThread);

    if (worker == NULL) {
        cerr << "error" << endl;
        return 1;
    }

    cout << "tread slepping..." << endl;
   // Sleep(time);

    ResumeThread(worker);

    cout << "threed is working" << endl;

    
   // WaitForSingleObject(worker, INFINITE);
    cout << "thread zavershon" << endl;

   
    delete[] str;
    delete s;
    CloseHandle(worker);

    return 0;
}
