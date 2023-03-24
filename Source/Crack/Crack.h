#ifndef SYM_CRACK
#define SYM_CRACK

#include <stdlib.h>

const unsigned long long right_hash_sum = 13624739409245231419ULL;

int CheckCrackFile(int argc, char* argv[], size_t* crack_file_length);

int Crack(char* argv[], size_t crack_file_length);

#endif
