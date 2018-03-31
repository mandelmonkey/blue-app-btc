 
#include "counterparty_utils.h"

static const int PREFIX_START = 0;
static const int PREFIX_LENGTH = 8;
 
static const int MESSAGE_START = 8;
//static const int MESSAGE_LENGTH = 1;

static const int ASSET_START = 9;
static const int ASSET_LENGTH = 8;

static const int AMOUNT_START = 17;
static const int AMOUNT_LENGTH = 8;
static const uint8_t AMOUNT_MAX_SIZE = 22;
//static const int NETWORK_START = 25;
//static const int NETWORK_LENGTH = 1;

static const int ADDRESS_START = 26;
static const int ADDRESS_LENGTH = 20;

 uint64_t convertToDecimal (unsigned char *array, int n)
{
   uint64_t result = 0;

   while (n--)
   {
       result <<= 8;
       result += *array;
       array++;
   }
   return result;
}
 

//stolen from stellar app
void print_long(uint64_t id, char *out) {
    char buffer[AMOUNT_MAX_SIZE];
    uint64_t dVal = id;
    int i, j;

    memset(buffer, 0, AMOUNT_MAX_SIZE);
    for (i = 0; dVal > 0; i++) {
        buffer[i] = (dVal % 10) + '0';
        dVal /= 10;
        if (i >= AMOUNT_MAX_SIZE) {
            THROW(0x6700);
        }
    }
    // reverse order
    for (i -= 1, j = 0; i >= 0 && j < AMOUNT_MAX_SIZE-1; i--, j++) {
        out[j] = buffer[i];
    }
    out[j] = '\0';
}
 
  
void unpack(unsigned char key[],int keySize, unsigned char data[],int dataSize, unsigned char *fullAmount,unsigned char *tokenName,unsigned char *cpHash160 ) {
 
    
    //arc4 decrypt the data
    unsigned char S[256];
 
    for(int i=0; i<256; i++) {
       
        S[i] = i;

    }

    
    for(int i=0,j=0; i<256; i++) {

        j = (j + S[i] + key[i % keySize]) % 256;
 
           int tmp1 = S[i];
        int tmp2 = S[j];
        S[i] = tmp2;
        S[j] = tmp1;
    
    }
  
    unsigned char ret[dataSize];
   
    
    for(int x=0,i=0,j=0; x<dataSize; x++) {
        
        i = (i + 1) % 256;
        
        j = (j + S[i]) % 256;
        
         int tmp1 = S[i];
        int tmp2 = S[j];
        S[i] = tmp2;
        S[j] = tmp1;

        int K = S[(S[i] + S[j]) % 256];
        
        ret[x] = (data[x] ^ K);
   
    }


    //When creating the cp message data I added a custom flag in the memo section to indicate if the token is divisible or not as this will change the amount format
    int MEMO_START = ADDRESS_START+ADDRESS_LENGTH;
    bool tokenIsDivisible = false;
    if(ret[MEMO_START] == 1){
       tokenIsDivisible = true;
    }

    
   
    //parse address data from decrypted data
  //  for(int i = ADDRESS_START; i < (ADDRESS_START+ADDRESS_LENGTH); i++) {
      
      
       
   // }

     os_memmove(cpHash160,ret+ADDRESS_START,ADDRESS_LENGTH);
     /*
       //need to add checksum
     cpAdd160[23]=00;
    cpAdd160[22]=00;
   cpAdd160[21]=00;
 cpAdd160[24]=00;
     
     b58enc(cpAddress, &cpAddressSize, cpAdd160,  sizeof(cpAdd160));*/

    

    //parse amount data from decrypted data
    unsigned char amount[8];
    /*for(int i = AMOUNT_START; i < (AMOUNT_START + AMOUNT_LENGTH); i++) {

        amount[i-AMOUNT_START]=ret[i];
         
    }*/
    os_memmove(amount,ret+AMOUNT_START,AMOUNT_LENGTH);



    if(tokenIsDivisible == false){ 
        /*
             //token is not divisble to convert byte arry hex number to hex number string
            unsigned char cpAmountHex[17];
            unsigned char i;
            
            for (i = 0; i < 8; i++) {
                snprintf(&cpAmountHex[i*2], sizeof(cpAmountHex), "%02x",  amount[i]);  
            }

            cpAmountHex[16]='\0';
            
            //convert hex number string to uint64 kinda long, 
            //TODO create value directly from byte array without converting to numeric hex string!

            uint64_t amountLong = hexdec(cpAmountHex);*/

            uint64_t  amountLong = convertToDecimal(amount,8);
            //convert long to string as ledger cant use snprintf with uint64_t
            print_long(amountLong,fullAmount);
 

        } else {

             //token is divisible so pass to ledger btc amount formater function to divide numeric hex string by 10^8 and convert to displayable string
           
            btchip_context_D.tmp = (unsigned char *)(fullAmount);
    
           unsigned short textSize = btchip_convert_hex_amount_to_displayable(amount);
            
           fullAmount[textSize] = '\0'; 
             

         } 
    
 
    unsigned char assetData[8];

    os_memmove(assetData,ret+ASSET_START,ASSET_LENGTH);

    //convert numeric hex string into long well uint64_t because ledger dont like longs
    uint64_t assetid = convertToDecimal(assetData,8);


    //counterparty assetid to assetname function

    char b26_digits[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
 
    int rem = 0;

    uint64_t div = 0;
  
    char asset_name_tmp[21];
     
    int nameLength = 0;

    while (assetid > 0) {

        div =  assetid/26;

        rem = assetid % 26;
       
        assetid = div;
       
        asset_name_tmp[nameLength] = b26_digits[rem];
      
        nameLength++;
    }

    //name is backwards so we need to reverse it
  
    for(int i=0;i<nameLength;i++) {
       
       tokenName[nameLength - (i+1)] = asset_name_tmp[i];
      
    } 

    //add string limiting null character
    tokenName[nameLength]='\0';
 
    return; 
      
};

 