
#include <eEEPROM.h>

typedef struct { float f; double d; } mystruct;

struct mydata { int  a; long b; mystruct c; } * EE = 0;
eEE_CHECKSIZE(*EE)

void setup()
{
    int  i = 4711;
    long l = 12345678;
    mystruct s = { f:3.14159265359,  d:2.71828182846 };
    
    Serial.begin(9600);

    PPRINT("Writing to EEProm");
    eEE_WRITE(i, EE->a); 
    eEE_WRITE(l, EE->b); 
    eEE_WRITE(s, EE->c); 
}

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

