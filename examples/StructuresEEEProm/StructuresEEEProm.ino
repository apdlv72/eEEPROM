/*
 * This sample demonstrates the basic usage of the round robin strategy for arbitrary data.
 * The macro eEE_rrstruct() will create internally an array of structures that consist
 * of a "current" byte, followed by the actual user data.
 * The macros eEE_WRITERRS() and eEE_WRITERRS() will read/write these values
 * and take care of finding/setting the "current" byte on this array.
 */

#include <eEEPROM.h>

typedef struct { int a; int b; } mystruct;

struct mydata 
{ 
	// this is a single integer
	eEE_rrint7_t(myInt,10);
	// this wraps the user defined structure in the array described above
	eEE_rrstruct(myStruct,10,mystruct);
} * EE = 0;

// check and raise an error if data exceeds EEProm capacity
eEE_CHECKSIZE(*EE);

// the setup routine runs once when you press reset:
void setup()
{
	Serial.begin(9600);

	mystruct init = { a:111, b:222 };

	eEE_WRITERRI(1000, EE->myInt);
	eEE_WRITERRS(init, EE->myStruct);
}

// the loop routine runs over and over again forever:
void loop()
{
	int      i;
	mystruct s;

	i = eEE_READRRI(EE->myInt);
	eEE_READRRS(EE->myStruct, s);

	Serial.println(i);	 // 100
	Serial.println(s.a); // 111
	Serial.println(s.b); // 222

	delay(10*1000);
}
