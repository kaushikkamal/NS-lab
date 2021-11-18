#include <stdio.h>
#include <stdlib.h>

#define SENTENCE 10
#define EN_FILENAME "encrypted.txt"
#define DE_FILENAME "decrypted.txt"

int deencode(int n, int key)
{
    return n << key;
}

int main()
{
    FILE *file, *wFile;
    char arr[SENTENCE][100];
    int asc_arr[SENTENCE][100];
    
    file = fopen("sen.txt", "r");
    wFile = fopen("encrypted.txt", "w");
}