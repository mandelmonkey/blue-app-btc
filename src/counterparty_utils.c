
#include "counterparty_utils.h"

static const uint8_t AMOUNT_MAX_SIZE = 22;

static const unsigned char PREFIX_START = 0;
static const unsigned char PREFIX_LENGTH = 8;

static const unsigned char MESSAGE_START = 8;
static const unsigned char MESSAGE_LENGTH = 1;

static const unsigned char GIVE_ASSET_START_ORDER = 9;
static const unsigned char GET_ASSET_START_ORDER = 25;

static const unsigned char ASSET_START_ENHANCED_SEND = 9;
static const unsigned char ASSET_LENGTH = 8;

static const unsigned char AMOUNT_START_ENHANCED_SEND = 17;
static const unsigned char AMOUNT_LENGTH = 8;


static const unsigned char GIVE_AMOUNT_START_ORDER = 17;
static const unsigned char GET_AMOUNT_START_ORDER = 33;


static const unsigned char ADDRESS_START_ENHANCED_SEND = 26;
static const unsigned char ADDRESS_LENGTH = 20;



uint64_t
convertToDecimal (unsigned char *array, unsigned char n)
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
unsigned short
print_long (uint64_t id, char *out)
{
  char buffer[AMOUNT_MAX_SIZE];
  uint64_t dVal = id;
  int i, j;

  memset (buffer, 0, AMOUNT_MAX_SIZE);
  for (i = 0; dVal > 0; i++)
    {
      buffer[i] = (dVal % 10) + '0';
      dVal /= 10;
      if (i >= AMOUNT_MAX_SIZE)
    {
      THROW (0x6700);
    }
    }
  // reverse order
  for (i -= 1, j = 0; i >= 0 && j < AMOUNT_MAX_SIZE - 1; i--, j++)
    {
      out[j] = buffer[i];
    }
  out[j] = '\0';
  return j;
}



void decodeAssetID (uint64_t assetID, unsigned char*out)
{
       //counterparty assetid to assetname function

      char b26_digits[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

      unsigned char rem = 0;

      uint64_t div = 0;

      char asset_name_tmp[21];

      unsigned char nameLength = 0;

      while (assetID > 0)
    {

      div = assetID / 26;

      rem = assetID % 26;

      assetID = div;

      asset_name_tmp[nameLength] = b26_digits[rem];

      nameLength++;
    }

      //name is backwards so we need to reverse it

      for (unsigned char i = 0; i < nameLength; i++)
    {

      out[nameLength - (i + 1)] = asset_name_tmp[i];

    }

      //add string limiting null character
      out[nameLength] = '\0';
}




int
unpack (unsigned char key[], unsigned char keySize, unsigned char data[],
    unsigned char dataSize, unsigned char *fullAmount,
    unsigned char *tokenName, unsigned char *cpHash160, unsigned char *getAsset, unsigned char *giveAsset, unsigned char *getAmount, unsigned char *giveAmount)
{


  //arc4 decrypt the data
  unsigned char S[256];

  for (int i = 0; i < 256; i++)
    {

      S[i] = i;

    }


  for (int i = 0, j = 0; i < 256; i++)
    {

      j = (j + S[i] + key[i % keySize]) % 256;

      int tmp1 = S[i];
      int tmp2 = S[j];
      S[i] = tmp2;
      S[j] = tmp1;

    }

  unsigned char ret[dataSize];


  for (int x = 0, i = 0, j = 0; x < dataSize; x++)
    {

      i = (i + 1) % 256;

      j = (j + S[i]) % 256;

      int tmp1 = S[i];
      int tmp2 = S[j];
      S[i] = tmp2;
      S[j] = tmp1;

      int K = S[(S[i] + S[j]) % 256];

      ret[x] = (data[x] ^ K);

    }

  int messageType = ret[MESSAGE_START];

  if (messageType == TYPE_ORDER)
    {


      unsigned char getAssetData[8];

      os_memmove (getAssetData, ret + GET_ASSET_START_ORDER, ASSET_LENGTH);

      //convert numeric hex string into long well uint64_t because ledger dont like longs
      uint64_t getAssetId = convertToDecimal (getAssetData, 8);

      decodeAssetID(getAssetId,getAsset);


      unsigned char giveAssetData[8];

      os_memmove (giveAssetData, ret + GIVE_ASSET_START_ORDER, ASSET_LENGTH);

      //convert numeric hex string into long well uint64_t because ledger dont like longs
      uint64_t giveAssetId = convertToDecimal (giveAssetData, 8);

      decodeAssetID(giveAssetId,giveAsset);




      //parse amount data from decrypted data
      unsigned char get_amount[8];
      
      os_memmove (get_amount, ret + GET_AMOUNT_START_ORDER, AMOUNT_LENGTH);

      uint64_t getAmountLong = convertToDecimal (get_amount, 8);

      //convert long to string as ledger cant use snprintf with uint64_t
      print_long (getAmountLong, getAmount);


       //parse amount data from decrypted data
      unsigned char give_amount[8];
      
      os_memmove (give_amount, ret + GIVE_AMOUNT_START_ORDER, AMOUNT_LENGTH);

      uint64_t giveAmountLong = convertToDecimal (give_amount, 8);

      //convert long to string as ledger cant use snprintf with uint64_t
      print_long (giveAmountLong, giveAmount);


     


    }
  else if (messageType == TYPE_ENHANCED_SEND)
    {


      

      os_memmove (cpHash160, ret + ADDRESS_START_ENHANCED_SEND, ADDRESS_LENGTH);


      //parse amount data from decrypted data
      unsigned char amount[8];
      
      os_memmove (amount, ret + AMOUNT_START_ENHANCED_SEND, AMOUNT_LENGTH);



     uint64_t amountLong = convertToDecimal (amount, 8);

      //convert long to string as ledger cant use snprintf with uint64_t
      print_long (amountLong, fullAmount);

  /*  //When creating the cp message data I added a custom flag in the memo section to indicate if the token is divisible or not as this will change the amount format
    int MEMO_START = ADDRESS_START_ENHANCED_SEND + ADDRESS_LENGTH;
    bool tokenIsDivisible = false;
      if (ret[MEMO_START] == 1)
    {
      tokenIsDivisible = true;
    }



      if (tokenIsDivisible == false)
    {

      uint64_t amountLong = convertToDecimal (amount, 8);


      //convert long to string as ledger cant use snprintf with uint64_t
      print_long (amountLong, fullAmount);



    }
      else
    {

      //token is divisible so pass to ledger btc amount formater function to divide numeric hex string by 10^8 and convert to displayable string

      btchip_context_D.tmp = (unsigned char *) (fullAmount);

      unsigned short textSize =
        btchip_convert_hex_amount_to_displayable (amount);

      fullAmount[textSize] = '*';   //add an asterix to show user that token is reported as divisible and has been divided by 10^8
      fullAmount[textSize + 1] = '\0';


        //if the user wants to send 1 INDIVISIBLETOKEN a hacker could change the transaction before it hits the ledger to 
        // send 100000000 INDIVISBLETOKEN and set the flag as divisible meaning the ledger app would show 1* the aterix should 
        // warn the user that the real amount has been dvided for display puposes which is strange as this is not the case for in divisible tokens 

    }*/






      unsigned char assetData[8];

      os_memmove (assetData, ret + ASSET_START_ENHANCED_SEND, ASSET_LENGTH);

      //convert numeric hex string into long well uint64_t because ledger dont like longs
      uint64_t assetid = convertToDecimal (assetData, 8);

      decodeAssetID(assetid,tokenName);


    }else if(messageType == TYPE_CANCEL){
        //do nothing
    }

  return messageType;

};
