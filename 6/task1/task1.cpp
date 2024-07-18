#pragma comment (lib, "winmm.lib")
#include <omp.h>
#include <iostream>
#include <format>
#include <string>
#include <windows.h>

int main(int argc, char* argv[])
{
    auto begin = timeGetTime();
    double pi = 0.0;
    long long count = std::stoull(argv[2]);
    unsigned mode = std::stoi(argv[1]);
    switch (mode)
    {
    case(1):
    {
        for (long long i = 0; i < count; i++)
        {
            pi += (pow(-1, i)) / (2 * i + 1);
            //sign = -sign;
        }
        break;
    }
    case(2):
    {
        #pragma omp parallel for 
        for (long long i = 0; i < count; i++)
        {
            pi += (pow(-1, i)) / (2 * i + 1);
        }
        break;
    }
    case(3):
    {
        #pragma omp parallel for 
        for (long long i = 0; i < count; i++)
        {
            #pragma omp atomic
            pi += (pow(-1, i)) / (2 * i + 1);
        }
        break;
    }
    case(4):
    {
        #pragma omp parallel for reduction(+:pi)
        for (long long i = 0; i < count; i++)
        {
            pi += (pow(-1, i)) / (2 * i + 1);
        }
        break;
    }
    default:
        break;
    }
    pi *= 4;
    std::cout << std::format("{}", pi) << std::endl;
    std::cout << timeGetTime() - begin << std::endl;
}
