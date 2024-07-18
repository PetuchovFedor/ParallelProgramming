#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include "tchar.h"

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	std::ostringstream os;
	os << "Поток №" << (int)lpParam << " выполняет свою работу" << std::endl;
	std::cout << os.str();
	ExitThread(0);
}

int _tmain(int argc, _TCHAR* argv[])
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(LC_ALL, "Russian");
	int N = _tstoi(argv[1]);
	HANDLE* handles = new HANDLE[N];
	for (size_t i = 0; i < N; i++)
	{
		handles[i] = CreateThread(NULL, 0, &ThreadProc, LPVOID(i + 1), CREATE_SUSPENDED, NULL);
	}
	
	for (size_t i = 0; i < N; i++)
	{
		ResumeThread(handles[i]);
	}

	WaitForMultipleObjects(N, handles, true, INFINITE);
	return 0;
}

