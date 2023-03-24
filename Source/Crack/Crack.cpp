#include "Crack.h"

#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>

int Crack(char* argv[], size_t crack_file_length)
{
    printf("length = %zu\n", crack_file_length);
    int fd = open(argv[1], O_RDWR);
    if (fd == -1)
        return -1;

    char* array = (char*)mmap(NULL, crack_file_length, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);

    array[0] = (char)0xbf;
    array[1] = (char)0xe8;
    array[2] = (char)0x01;
    array[3] = (char)0xff;
    array[4] = (char)0xe7;

    for(int i = 0xfe; i <= 0x113; i++)
        array[i] = (char)0x90;
    
    if (munmap(array, crack_file_length) == -1)
        return -1;

    return 0;
}

int CheckCrackFile(int argc, char* argv[], size_t* crack_file_length)
{
    if (crack_file_length == nullptr)
    {
        printf("crack file length = nullptr\n");
        return -1;
    }
    if (argc != 2)
    {
        printf("Error\n");
        return -1;
    }
    
    FILE* fp = fopen(argv[1], "rb");
    if (fp == nullptr)
    {
        printf("Error during opening file\n");
        return -1;
    }

    int                c        = 0;
    unsigned long long hash_sum = 5531;

    *crack_file_length = 0;
    while ((c = getc(fp)) != EOF)
    {
        hash_sum = hash_sum*11 + (unsigned long long)c;
        (*crack_file_length)++;
    }

    fclose(fp);

    printf("hash_sum = %llu\n", hash_sum);
    if (hash_sum != right_hash_sum)
    {
        printf("Wrong file\n");
        return -1;
    }

    return 0;
}
