/*
 * This sample demonstrates how to organize your data that needs to go into EEProm
 * in a structure, have the compiler check whether its size exceeds EEProm capacity
 * and how to read and write this data conveniently.
 */

#include <eEEPROM.h>

// user defined data
typedef struct { float f; double d; } mystruct;

// everything that needs to go to EEProm
struct mydata { int  a; long b; mystruct c; } * EE = 0;

// check and raise an error if data exceeds EEProm capacity
eEE_CHECKSIZE(*EE)

// the setup routine runs once when you press reset:
void setup()
{
    int  i = 4711;
    long l = 12345678;
    mystruct s = { f:3.14159265359,  d:2.71828182846 }; // pi, e
    
    Serial.begin(9600);

    // PPRINT will place the given string into flash and save RAM.
    // Ignore compiler warnings regarding that "only initialized variables can be placed into program memory area"
    // This is because of an gcc bug
    PPRINT("Writing to EEProm");
    eEE_WRITE(i, EE->a); 
    eEE_WRITE(l, EE->b); 
    eEE_WRITE(s, EE->c); 
}

// the loop routine runs over and over again forever:
void loop()
{
    int    i;
    long   l;
    float  f;
    double d;
  
    eEE_READ(EE->a,   i); 
    eEE_READ(EE->b,   l); 
    eEE_READ(EE->c.f, f); 
    eEE_READ(EE->c.d, d); 
    
    PPRINT("Read i:"); Serial.println(i);
    PPRINT("Read l:"); Serial.println(l);
    PPRINT("Read f:"); Serial.println(f);
    PPRINT("Read d:"); Serial.println(d);
}

