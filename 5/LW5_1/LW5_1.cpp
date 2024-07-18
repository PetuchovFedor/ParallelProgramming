#include <windows.h>
#include <string>
#include <iostream>
#include "tchar.h"
#include <fstream>

CONST HANDLE mutex = CreateMutex(NULL, FALSE, (LPWSTR)"Mutex");

int ReadFromFile() {
	//WaitForSingleObject(mutex, INFINITE);
	std::fstream myfile("balance.txt", std::ios_base::in);
	int result;
	myfile >> result;
	myfile.close();
	//ReleaseMutex(mutex);
	return result;
}

void WriteToFile(int data) {
	//WaitForSingleObject(mutex, INFINITE);
	std::fstream myfile("balance.txt", std::ios_base::out);
	myfile << data << std::endl;
	myfile.close();
	//ReleaseMutex(mutex);
}

int GetBalance() {
	int balance = ReadFromFile();
	return balance;
}

void Deposit(int money) {
	auto result = WaitForSingleObject(mutex, INFINITE);
	int balance = GetBalance();
	balance += money;
	WriteToFile(balance);
	printf("Balance after deposit: %d\n", balance);
	ReleaseMutex(mutex);

}

void Withdraw(int money) {

	auto result = WaitForSingleObject(mutex, INFINITE);
	int balance = GetBalance();

	if (balance < money) {
		printf("Cannot withdraw money, balance lower than %d\n", money);
		//ReleaseMutex(mutex);
		return;
	}

	//Sleep(20);
	balance -= money;
	WriteToFile(balance);
	printf("Balance after withdraw: %d\n", balance);
	ReleaseMutex(mutex);
}

DWORD WINAPI DoDeposit(CONST LPVOID lpParameter)
{
	Deposit((int)lpParameter);
	ExitThread(0);
}

DWORD WINAPI DoWithdraw(CONST LPVOID lpParameter)
{
	Withdraw((int)lpParameter);
	ExitThread(0);
}

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE* handles = new HANDLE[49];

	//InitializeCriticalSection(&FileLockingCriticalSection);

	WriteToFile(0);
	SetProcessAffinityMask(GetCurrentProcess(), 1);
	for (int i = 0; i < 50; i++) {
		handles[i] = (i % 2 == 0)
			? CreateThread(NULL, 0, &DoDeposit, (LPVOID)230, CREATE_SUSPENDED, NULL)
			: CreateThread(NULL, 0, &DoWithdraw, (LPVOID)1000, CREATE_SUSPENDED, NULL);
		ResumeThread(handles[i]);
	}


	// ожидание окончания работы двух потоков
	WaitForMultipleObjects(50, handles, true, INFINITE);
	printf("Final Balance: %d\n", GetBalance());

	getchar();
	CloseHandle(mutex);
	//DeleteCriticalSection(&FileLockingCriticalSection);

	return 0;
}
