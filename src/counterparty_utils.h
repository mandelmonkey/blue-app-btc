 #include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "os.h" 
#include <stdbool.h>
#include "btchip_internal.h"

void unpack(unsigned char key[],int keySize, unsigned char data[],int dataSize, unsigned char *fullAmount,unsigned char *tokenName,unsigned char *cpHash160);
void print_long(uint64_t id, char *out);
uint64_t hexdec(unsigned const char *hex);

 