#pragma once
#include <jadel.h>

inline void swapFloat(float *f0, float *f1)
{
    float temp = *f0;
    *f0 = *f1;
    *f1 = temp;
}

inline void swapFloatIfHigherToLower(float *f0, float *f1)
{
    if (*f0 > *f1)
    {
        swapFloat(f0, f1);
    }
}