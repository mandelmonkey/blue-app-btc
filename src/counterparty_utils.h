 #include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "os.h" 
#include <stdbool.h>
#include "btchip_internal.h"

static const unsigned char TYPE_ORDER = 10;
static const unsigned char TYPE_ENHANCED_SEND = 2;
static const unsigned char TYPE_CANCEL = 70;

int unpack(unsigned char key[],unsigned char keySize, unsigned char data[],unsigned char dataSize, unsigned char *fullAmount,unsigned char *tokenName,unsigned char *cpHash160,unsigned char *getAsset,unsigned char *giveAsset,unsigned char *getAmount,unsigned char *giveAmount);
