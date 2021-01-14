#include <windows.h>
#include <iostream>
#include "lab3.h"

using namespace std;

#define THREADCOUNT 12
DWORD ThreadID;
HANDLE iThread[THREADCOUNT];
HANDLE iMutex;
HANDLE semG, semH, semK;
HANDLE start, stop;

DWORD WINAPI Thread_a(LPVOID);
DWORD WINAPI Thread_b(LPVOID);
DWORD WINAPI Thread_c(LPVOID);
DWORD WINAPI Thread_d(LPVOID);
DWORD WINAPI Thread_e(LPVOID);
DWORD WINAPI Thread_f(LPVOID);
DWORD WINAPI Thread_g(LPVOID);
DWORD WINAPI Thread_g_semaphore(LPVOID);
DWORD WINAPI Thread_h(LPVOID);
DWORD WINAPI Thread_h_semaphore(LPVOID);
DWORD WINAPI Thread_i(LPVOID);
DWORD WINAPI Thread_k(LPVOID);
DWORD WINAPI Thread_k_semaphore(LPVOID);
DWORD WINAPI Thread_m(LPVOID);
DWORD WINAPI Thread_n(LPVOID);

unsigned int lab3_thread_graph_id() 
{
    return 5;
}

const char* lab3_unsynchronized_threads()
{
    return "bceg";
}

const char* lab3_sequential_threads()
{
    return "ghk";
}

int lab3_init()
{
    iMutex = CreateMutex(NULL, 0, NULL);

    if (iMutex == NULL)
    {
        std::cout << "ERROR - Create Mutex: " << GetLastError();
        return 1;
    }

    semG = CreateSemaphore(NULL, 1, 1, NULL);
    if (semG == NULL)
    {
        std::cout << "ERROR - Create Semaphore G: " << GetLastError();
        return 1;
    }
    semH = CreateSemaphore(NULL, 0, 1, NULL);
    if (semH == NULL)
    {
        std::cout << "ERROR - Create Semaphore H: " << GetLastError();
        return 1;
    }
    semK = CreateSemaphore(NULL, 0, 1, NULL);
    if (semK == NULL)
    {
        std::cout << "ERROR - Create Semaphore K: " << GetLastError();
        return 1;
    }

    stop = CreateSemaphore(NULL, 0, 2, NULL);
    if (stop == NULL)
    {
        std::cout << "CreateSemaphore error stop: " << GetLastError();
        return 1;
    }

    start = CreateSemaphore(NULL, 0, 2, NULL);
    if (start == NULL)
    {
        std::cout << "CreateSemaphore error start: " << GetLastError();
        return 1;
    }

    //NULL аттрибуты безопасности по умолчанию
    //0 | 1   начальное значение счетчика
    //0 | 1   максимаьлное значение счетчика
    //NULL безымянный семафор

    //THREAD C
    iThread[0] = CreateThread(NULL, 0, thread_c, NULL, 0, &ThreadID);
    // (1)NULL - аттрибуты безопасности
    // (2)0 - размер стека
    // (4)NULL - нет аргумента для функ.- тела потока
    // (5)0 - флаг соз. потока

    if (iThread[0] == NULL)
    {
        std::cout << "ERROR - Create Thread c: " << GetLastError();
        return 1;
    }

    WaitForSingleObject(iThread[0], INFINITE); //a wait

    CloseHandle(iThread[0]); // закрываем дескриптор потока

    CloseHandle(iMutex);
    CloseHandle(stop);
    CloseHandle(start);

    CloseHandle(semG);
    CloseHandle(semH);
    CloseHandle(semK);

    return 0;
}

DWORD WINAPI thread_c(LPVOID lpParam) {
    //interval 1
     //create a
    iThread[1] = CreateThread(NULL, 0, thread_a, NULL, 0, &ThreadID);
    if (iThread[1] == NULL) {
        return GetLastError();
    }

    //interval 1
    for (int i = 0; i < 3; i++) {
        WaitForSingleObject(iMutex, INFINITE);
        cout << "c" << flush;
        computation();
        ReleaseMutex(iMutex);
    }

    // wait-a
    WaitForSingleObject(iThread[1], INFINITE);

    //interval2

    iThread[2] = CreateThread(NULL, 0, thread_g, NULL, 0, &ThreadID);
    if (iThread[2] == NULL) {
        return GetLastError();
    }

    iThread[3] = CreateThread(NULL, 0, thread_e, NULL, 0, &ThreadID);
    if (iThread[3] == NULL) {
        return GetLastError();
    }

    iThread[4] = CreateThread(NULL, 0, thread_b, NULL, 0, &ThreadID);
    if (iThread[4] == NULL) {
        return GetLastError();
    }

    for (int i = 0; i < 3; i++) {
        WaitForSingleObject(iMutex, INFINITE);
        cout << "c" << flush;
        computation();
        ReleaseMutex(iMutex);
    }

    //wait b,e
    WaitForSingleObject(iThread[3], INFINITE);
    WaitForSingleObject(iThread[4], INFINITE);

    //wait g
    WaitForSingleObject(stop, INFINITE);

    //interval 3
    iThread[5] = CreateThread(NULL, 0, thread_f, NULL, 0, &ThreadID);
    if (iThread[5] == NULL) {
        return GetLastError();
    }

    iThread[6] = CreateThread(NULL, 0, thread_d, NULL, 0, &ThreadID);
    if (iThread[6] == NULL) {
        return GetLastError();
    }

    //start g
    ReleaseSemaphore(start, 1, NULL);

    for (int i = 0; i < 3; i++) {
        WaitForSingleObject(iMutex, INFINITE);
        cout << "c" << flush;
        computation();
        ReleaseMutex(iMutex);
    }

    //wait g,f,d
    WaitForSingleObject(stop, INFINITE);
    WaitForSingleObject(iThread[5], INFINITE);
    WaitForSingleObject(iThread[6], INFINITE);

    //create k
    iThread[7] = CreateThread(NULL, 0, thread_k, NULL, 0, &ThreadID);
    if (iThread[7] == NULL) {
        return GetLastError();
    }

    WaitForSingleObject(iThread[7], INFINITE);
    return 0;
}

DWORD WINAPI thread_a(LPVOID lpParam) {
    for (int i = 0; i < 3; i++) {
        WaitForSingleObject(iMutex, INFINITE);
        cout << "a" << flush;
        computation();
        ReleaseMutex(iMutex);
    }
    return 0;
}

DWORD WINAPI thread_e(LPVOID lpParam) {
    for (int i = 0; i < 3; i++) {
        WaitForSingleObject(iMutex, INFINITE);
        cout << "e" << flush;
        computation();
        ReleaseMutex(iMutex);
    }
    return 0;
}

DWORD WINAPI thread_b(LPVOID lpParam) {
    for (int i = 0; i < 3; i++) {
        WaitForSingleObject(iMutex, INFINITE);
        cout << "b" << flush;
        computation();
        ReleaseMutex(iMutex);
    }
    return 0;
}

DWORD WINAPI thread_f(LPVOID lpParam) {
    for (int i = 0; i < 3; i++) {
        WaitForSingleObject(iMutex, INFINITE);
        cout << "f" << flush;
        computation();
        ReleaseMutex(iMutex);
    }
    return 0;
}

DWORD WINAPI thread_d(LPVOID lpParam) {
    for (int i = 0; i < 3; i++) {
        WaitForSingleObject(iMutex, INFINITE);
        cout << "d" << flush;
        computation();
        ReleaseMutex(iMutex);
    }
    return 0;
}

DWORD WINAPI thread_g(LPVOID lpParam) {

    //interval 1

    for (int i = 0; i < 3; i++) {
        WaitForSingleObject(iMutex, INFINITE);
        cout << "d" << flush;
        computation();
        ReleaseMutex(iMutex);
    }

    ReleaseSemaphore(stop, 1, NULL);
    WaitForSingleObject(start, INFINITE);

    //interval 2
    for (int i = 0; i < 3; i++) {
        WaitForSingleObject(iMutex, INFINITE);
        cout << "d" << flush;
        computation();
        ReleaseMutex(iMutex);
    }

    ReleaseSemaphore(stop, 1, NULL);
    WaitForSingleObject(start, INFINITE);

    //interval3
    for (int i = 0; i < 3; i++) {
        WaitForSingleObject(semG, INFINITE);
        WaitForSingleObject(iMutex, INFINITE);
        cout << "d" << flush;
        computation();
        ReleaseMutex(iMutex);
        ReleaseSemaphore(semH, 1, NULL);
    }

    ReleaseSemaphore(stop, 1, NULL);
    return 0;
}

DWORD WINAPI thread_k(LPVOID lpParam) {

    iThread[8] = CreateThread(NULL, 0, thread_h, NULL, 0, &ThreadID);
    if (iThread[8] == NULL) {
        return GetLastError();
    }

    //start g
    ReleaseSemaphore(start, 1, NULL);

    //interval 1
    for (int i = 0; i < 3; i++) {
        WaitForSingleObject(semK, INFINITE);
        WaitForSingleObject(iMutex, INFINITE);
        cout << "k" << flush;
        computation();
        ReleaseMutex(iMutex);
        ReleaseSemaphore(semG, 1, NULL);
    }

    //wait h, g
    WaitForSingleObject(stop, INFINITE);
    WaitForSingleObject(iThread[5], INFINITE);
  

    //interval2
    iThread[9] = CreateThread(NULL, 0, thread_i, NULL, 0, &ThreadID);
    if (iThread[9] == NULL) {
        return GetLastError();
    }
    iThread[10] = CreateThread(NULL, 0, thread_m, NULL, 0, &ThreadID);
    if (iThread[10] == NULL) {
        return GetLastError();
    }

    for (int i = 0; i < 3; i++) {
        WaitForSingleObject(iMutex, INFINITE);
        cout << "k" << flush;
        computation();
        ReleaseMutex(iMutex);
    }

    //wait i, m
    WaitForSingleObject(iThread[10], INFINITE);
    WaitForSingleObject(stop, INFINITE);

    iThread[11] = CreateThread(NULL, 0, thread_n, NULL, 0, &ThreadID);
    if (iThread[11] == NULL) {
        return GetLastError();
    }

    //wait n
    WaitForSingleObject(iThread[11], INFINITE);
    return 0;
}

DWORD WINAPI thread_h(LPVOID lpParam) {

    for (int i = 0; i < 3; i++) {
        WaitForSingleObject(semH, INFINITE);
        WaitForSingleObject(iMutex, INFINITE);
        cout << "h" << flush;
        computation();
        ReleaseMutex(iMutex);
        ReleaseSemaphore(semK, 1, NULL);
    }

    return 0;
}

DWORD WINAPI thread_i(LPVOID lpParam) {
    for (int i = 0; i < 3; i++) {
        WaitForSingleObject(iMutex, INFINITE);
        cout << "i" << flush;
        computation();
        ReleaseMutex(iMutex);
    }
    return 0;
}

DWORD WINAPI thread_n(LPVOID lpParam) {

    //start m
    ReleaseSemaphore(start, 1, NULL);

    for (int i = 0; i < 3; i++) {
        WaitForSingleObject(iMutex, INFINITE);
        cout << "n" << flush;
        computation();
        ReleaseMutex(iMutex);
    }
    return 0;
}

DWORD WINAPI thread_m(LPVOID lpParam) {

    //interval 1
    for (int i = 0; i < 3; i++) {
        WaitForSingleObject(iMutex, INFINITE);
        cout << "n" << flush;
        computation();
        ReleaseMutex(iMutex);
    }

    ReleaseSemaphore(stop, 1, NULL);
    WaitForSingleObject(start, INFINITE);

    //interval 1
    for (int i = 0; i < 3; i++) {
        WaitForSingleObject(iMutex, INFINITE);
        cout << "n" << flush;
        computation();
        ReleaseMutex(iMutex);
    }

    return 0;
}