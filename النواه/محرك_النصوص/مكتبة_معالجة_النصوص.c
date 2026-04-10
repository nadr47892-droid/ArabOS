#include "مكتبة_معالجة_النصوص.h"

int strcmp(const char* a, const char* b)
{
    int i = 0;

    while (a[i] && b[i]) {

        if (a[i] != b[i])
            return a[i] - b[i];

        i++;
    }

    return a[i] - b[i];
}

void strcpy(char* dest, const char* src)
{
    int i = 0;

    while (src[i]) {

        dest[i] = src[i];
        i++;
    }

    dest[i] = 0;
}

int strlen(const char* s)
{
    int i = 0;

    while (s[i])
        i++;

    return i;
}
