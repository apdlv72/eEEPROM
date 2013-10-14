eEEPROM
=======

Arduino library to ease the use of the uC's EEPROM. The "e" in eEEPROM stands for "easy".
It does so by providing some functions and macros to write more complex data than just
single bytes. 

Moreover, this library also tries to reduce write operations to the EEPROM by comparing target 
and new value before every write. The number of write cycles to EEPROM ususally is limited 
to as few as 100k before content becomes unreliable.

Finally, it adds a macro to conveniently check if what has been defined for the EEProm exceed
its capacity. 

To make these macros macros work, you must define a structure that wraps everything you
need to store in EEProm as and a pointer to address 0.

  struct mydata { int  a; long b; struct s {} .... } * EE = 0;
  eEE_CHECKSIZE(*EE)

The macro eePROM_CHECKSIZE will fail with an error message, when the total size of the
structure exceeds the maximum EEProm capacity. The error message however is a bit cryptic.
It says something like "verflow in array dimension" - but that is much better than running
into funny problems at runtime anyway.

Reading and writing the tokens in EEProm then will become merely a childs play without the 
hassle of computing addreses and data sizes:

long l;
eEE_WRITE(l, EE->b);

int i;
eEE_READ(EE->a, i);

