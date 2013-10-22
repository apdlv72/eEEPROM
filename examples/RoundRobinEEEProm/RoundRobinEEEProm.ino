/*
 * This sample demonstrates the basic usage of the round robin integer arrays.
 * These abstract data types implement a single signed integers with 7, 15, 31 precision.
 * However, write cycles will be distributed equally over an array of configurable size
 * thus reducing writes to a single cell and therefore extend the life time of the EEProm.
 */
#include <eEEPROM.h>

// this structure contains everything that should go to EEProm
struct mydata 
{ 
  // a single 7-bit integer with 100k*200/2 = 10 mio. write cycles consuming 200 bytes in EEProm
  eEE_rrint7_t(myA,200);
  // a single 15-bit integer with 100k*10/2 = 500k write cycles consuming 20 bytes in EEProm
  eEE_rrint15_t(myB,5);
} * EE = 0;

// check and raise an error if data exceeds EEProm capacity
eEE_CHECKSIZE(*EE);

// the setup routine runs once when you press reset:
void setup()
{
    Serial.begin(9600);

    // initialize the counter. actually this should be done once only.
    // for simplicity, we do initialize upon every restart of the device.
    eEE_WRITERRI( 100, EE->myA);
    eEE_WRITERRI(1000, EE->myB);
}

// the loop routine runs over and over again forever:
void loop()
{
	// read the values from EEProm
    int8_t  a = eEE_READRRI(EE->myA);
    int16_t b = eEE_READRRI(EE->myB);

    Serial.println(a);
    Serial.println(b);

    delay(10*1000);
}
