﻿#include <omp.h>
#include <stdio.h>

int main()
{
    int x = 44;
    #pragma omp parallel for lastprivate(x)
    for (int i = 0; i <= 10; i++) {
        #pragma omp atomic
        x += i ;
        printf("Thread number: %d x: %d\n", omp_get_thread_num(), x);
    }
    printf("x is %d\n", x);
}
