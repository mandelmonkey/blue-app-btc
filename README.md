# Counterparty ledger nano s app (not ledger blue)
This is a fork of the ledger blue-app-btc with added support to parse counterparty transactions.

Supported transaction types are 
 - Enhanced Send
 - Order
 - Cancel

# Rational
Although it is possible to sign a counterparty transaction using the normal btc ledger app the users cannot be sure the transaction they are signing is the one they created, it may be possible for a man in the middle attack to change the transaction data before it reaches the ledger. This app will decode the op return data and display to the user exactly what is being signed.

# Caveats
Divisible tokens will show in satoshi value, for example if you intend to send 1 XCP, the ledger will show 100000000 XCP, this is because the amount for divisible tokens is decoded in the satoshi amount in counterparty transactions. It would be possible to pass extra info to the ledger so it can foramt the divisible token amount correctly however as this data would not be present in the transaction data being signed it cannot be trusted and would defeat the point of the added security gained by displaying the transaction information on the ledger.

# How to install
## Build from source
Clone this repo, install ledger nano sdk and bolos env outlined here
http://ledger.readthedocs.io/en/latest/userspace/getting_started.html 
and run 
```COIN=counterparty make load```
to delete the app run
```COIN=counterparty make delete```

## Install from binary
To install you must use the ledgerblue python loader with the binary 
https://github.com/LedgerHQ/blue-loader-python

download the app.hex and app.map file from releases and run the following command
```
python -m ledgerblue.loadApp --curve secp256k1 --tlv --targetId 0x31100003 --fileName app.hex --appName "Counterparty" --appVersion 1.2.4 --appFlags 0x850 --icon "0100000000ffffff00ffffffffffffffff8fe387c1c3c07bdc3bde03c387e1fff9ffffffffffffffff" --path "" --dataSize `cat app.map |grep _nvram_data_size | tr -s ' ' | cut -f2 -d' '`
```

to delete
```
python -m ledgerblue.deleteApp --targetId 0x31100003 --appName "Counterparty" 
```

# How to use
This app currently works in conjuction with the webapp https://ledger-cp.herokuapp.com

# Other information
More information can be found on the original blue-app-btc repo
https://github.com/LedgerHQ/blue-app-btc
