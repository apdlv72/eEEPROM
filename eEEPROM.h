/*
Copyright (C) Artur Pogoda de la Vega

Arduino library to ease the use of the uC's EEPROM. 
It does so by providing some functions and macros to write more complex data than just
singlebyets. The "e" in eEEPROM stands for "easy" and the naming claims to differ as little
as possible form the "normal" Arduino EEPROM library such that one can switch back and forth
will little effort should this become necessary for some reasons, e.g. bugs or memory footprint

Moreover, this library also tries to avoid write cycles to the EEPROM by comparing target 
value and new value before every write. Number of write cycles to EEPROM ususally is limited 
to as few as 100k before content becomes unreliable. The writes will be slightly slower
because of the extra overhead to read before write when there really was a change but remain the
same if there was no change, Should this be a problem anyway, the doWrite() method can be used
to force unconditional writing to the EEPROM.

Finally, it adds a macro to conveniently check if what has been defined for the EEProm exceed
its capacity. 

To make these macros macros work, you must define a structure that wraps everything you
need to store in EEProm as and a pointer to address 0.

struct mydata { int  a; long b; } * EE = 0;
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

*/

#ifndef eEEPROM_h
#define eEEPROM_h

#include <inttypes.h>

class eEEPROMClass
{
  public:
    uint8_t read(int addr);
    void write(int addr, uint8_t);
    
    // do not check target value but write through to the  EEPROM
    void doWrite(int addr, uint8_t value);

    uint16_t readWord(int addr);
    void writeWord(int addr, uint16_t);
    
    uint32_t readLong(int addr);
    void writeLong(int addr, uint32_t);
    
    float readFloat(int addr);
    void writeFloat(int addr, float);

    double readDouble(int addr);
    void writeDouble(int addr, double);
    
    void readData(int addr, void * buf, int len);
    void writeData(int addr, const void * buf, int len);
    
    void memFill(int addr, uint8_t data, uint16_t len);
};

extern eEEPROMClass eEEPROM;

#define eEE_ADDR(EETOKEN) ((uint16_t)(void*)&(EETOKEN))
#define eEE_WRITE(SRC, EETOKEN) { eEEPROM.writeData(eEE_ADDR(EETOKEN), &(SRC),  sizeof(EETOKEN)); }
#define eEE_READ(EETOKEN, DEST) { eEEPROM.readData( eEE_ADDR(EETOKEN), &(DEST), sizeof(DEST));    }
#define eEE_ZERO(EETOKEN)       { eEEPROM.memFill(eEE_ADDR(EETOKEN), 0, sizeof(EETOKEN)); }

#define eEE_CHECKSIZE(DATA) struct FailOnEEPromExceess { int c[E2END-sizeof(DATA)]; }; 

// eEEPROM_nh
#endif
