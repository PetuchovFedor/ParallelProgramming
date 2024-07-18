#pragma comment (lib, "winmm.lib")
#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include "tchar.h"
#include <fstream>

double GLOBAL_VARIABLE = 0.0;
const double PI = std::atan(1.0) * 4;
struct ToProc
{
	DWORD beginTime;
	int numThread;
	ToProc(DWORD beg, int n)
		: beginTime(beg)
		, numThread(n)
	{}
};

void WriteToFile(std::string filename, std::string data)
{
	std::ofstream file;
	file.open(filename, std::ios_base::app);
	if (file.is_open())
	{
		file << data << std::endl;
		file.close();
	}
	else
	{
		std::cout << "Ошибка при открытии файла " << filename << std::endl;
	}
}

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	ToProc* tmp = (ToProc*)lpParam;
	
	for (size_t k = 0; k < 20; k++)
	{
		for (size_t i = 0; i < 1000000; i++)
		{
			GLOBAL_VARIABLE += 30 * std::sin(GLOBAL_VARIABLE * PI / 180);
		}
		auto duration = timeGetTime() - tmp->beginTime;
		std::string data = std::to_string(tmp->numThread) + "|" + std::to_string(duration);
		WriteToFile("output1.txt", data);
	}
	ExitThread(0);
}

int main()
{
	char ch;
	std::cin >> ch;
	std::ofstream out("output1.txt");
	out.close();
	HANDLE* handles = new HANDLE[2];
	auto begin = timeGetTime();
	ToProc t1(begin, 1);
	ToProc t2(begin, 2);
	handles[0] = CreateThread(NULL, 0, &ThreadProc, LPVOID(&t1), CREATE_SUSPENDED, NULL);
	handles[1] = CreateThread(NULL, 0, &ThreadProc, LPVOID(&t2), CREATE_SUSPENDED, NULL);
	SetThreadPriority(handles[0], THREAD_PRIORITY_HIGHEST);
	ResumeThread(handles[0]);
	ResumeThread(handles[1]);
	WaitForMultipleObjects(2, handles, true, INFINITE);
	return 0;
}

