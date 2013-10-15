/*
Copyright (C) Artur Pogoda de la Vega

Arduino library to ease the use of the uC's EEPROM. 
It does so by providing some functions and macros to write more complex data than just
single bytes. The "e" in eEEPROM stands for "easy" and the naming claims to differ as little
as possible form the "normal" Arduino EEPROM library such that one can switch back and forth
will little effort should this become necessary for some reasons, e.g. bugs or memory footprint

Moreover, this library tries to avoid write cycles to the EEPROM by comparing the target 
value and new value before every write. The total number of write cycles to EEPROM ususally is limited 
to as few as 100k before content becomes unreliable. The writes will be slightly slower
because of the extra overhead to read before writing when there really was a change but remains the
same if there was no change, Should this be a problem anyway, the doWrite() method can be used
to force unconditional writing to the EEPROM.

Beside the macros to conveniently read and write EEProm data, there is also a macro to check if the
content that should reside in the EEProm exceed its maximum capacity.

To make all these macros macros work, you must define a structure that wraps everything you
need to store in EEProm as a pointer to address 0.

	struct mydata { int  a; long b; } * EE = 0;
	eEE_CHECKSIZE(*EE) // No semicolon here

The macro eEE_CHECKSIZE will fail with an error message, when the total size of the
structure exceeds the maximum EEProm capacity. The error message however is a bit cryptic.
It says something like "overflow in array dimension" - but that is much better than running
into funny problems at runtime anyway.

Reading and writing the tokens in EEProm then will become merely a childs play without the 
hassle of computing addreses and data sizes:

	long l;
	eEE_WRITE(l, EE->b);

	int i;
	eEE_READ(EE->a, i);

Last but not least there is a macro that actually is kind off-topic, because it does not deal
with data in EEProm but in Flash. However, because it is very useful to circumvent RAM limitations,
when your sketch grows larger on the one hand, and it is too tiny to put it into a separate library,
I nevertheless included  it in to this class.

When you put lots of Serial.print("Some static text") into your program you will run into apparently
non-deterministic behavior of your code or your sketch even town start at at. The reason is that
string like that will reside in RAM and eat up this memory for your program code. The macro PPRINT()
will put these strings into flash. (Actually they have been there anyway and copied into RAM at
boot time). Just use

	PPRINT("Some static text")

whenever you need to output some static string instead of Serial.print(). This should significantly
reduce RAM use if you have lots of these string. You can check the effects with the "avr-size"
command line tool included in your Arduino environment.
There is a nice tutorial here:

http://www.leonardomiliani.com/2012/come-sapere-loccupazione-di-ram-del-proprio-sketch/?lang=en

*/

#ifndef eEEPROM_h
#define eEEPROM_h

#include <inttypes.h>
#include <avr/pgmspace.h>
#include <Arduino.h>

/*
 * Structure to store integer values round robin.
 * The MSB bit defines which value is the current one and the remaining bits store the actual value.
 * Using an array of such structures and moving the current flag to the next element upon every
 * change will spread write cycles to the EEProm equally over the whole array.
 * This improves lifetime of the EEProm by N/2 where N is the number of array elements
 * (Not by N, because the former value needs to be invalidated).
 * Static member TYPE  is actually only a hint that allow the the template methods to derive
 * the correct type of the member "value".
 */
typedef struct { int8_t  current : 1; int8_t  value :  7; static int8_t  TYPE; } s_rrint7;
typedef struct { int16_t current : 1; int16_t value : 15; static int16_t TYPE; } s_rrint15;
typedef struct { int32_t current : 1; int32_t value : 31; static int32_t TYPE; } s_rrint31;
typedef struct { uint8_t current; uint8_t data; } s_rrstruct;

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

    void rrWriteStruct(s_rrstruct * rrstruct, uint16_t size, uint16_t count, void * user_struct);
    void rrReadStruct(s_rrstruct * rrstruct, uint16_t size, uint16_t count, void * user_struct);

    // template RRINT will become one of s_rrint{7|15|31} through the macro eEE_RRWRITE
    template <class RRINT> void rrWrite(RRINT * rrint, uint16_t count, __decltype(RRINT::TYPE) value)
    {
    	RRINT temp, next;
		next.current = 1;
		next.value   = value;

    	for (uint16_t i=0; i<count; i++)
    	{
        	uint16_t addr1 = (uint16_t)&rrint[i];
    		readData(addr1, &temp, sizeof(temp));
    		if (temp.current)
    		{
    			if (temp.value==value) return; // no change

    			uint16_t addr2 = (uint16_t)&rrint[(i+1)%count];
    			temp.current = 0;

    			writeData(addr2, &next, sizeof(next));
    			writeData(addr1, &temp, sizeof(temp));
    			return;
    		}
    	}

    	uint16_t addr = (uint16_t)rrint;
    	writeData(addr, &next, sizeof(next));
    }

    // template RRINT will become one of s_rrint{7|15|31} through the macro eEE_RRREAD
    template <class RRINT> __decltype(RRINT::TYPE	) rrRead(RRINT * rrint, uint16_t count)
    {
    	uint16_t addr = (uint16_t)rrint;
    	RRINT temp;
    	for (uint16_t i=0; i<count; i++, addr+=sizeof(s_rrint7))
    	{
    		readData(addr, &temp, sizeof(temp));
    		if (temp.current) return temp.value;
    	}
    	return 0;
    }

    void showPgmString (PGM_P s);
};

extern eEEPROMClass eEEPROM;

#define eEE_ADDR(EETOKEN) ((uint16_t)(void*)&(EETOKEN))
#define eEE_WRITE(SRC, EETOKEN) { eEEPROM.writeData(eEE_ADDR(EETOKEN), &(SRC),  sizeof(EETOKEN)); }
#define eEE_READ(EETOKEN, DEST) { eEEPROM.readData( eEE_ADDR(EETOKEN), &(DEST), sizeof(DEST));    }
#define eEE_ZERO(EETOKEN)       { eEEPROM.memFill(eEE_ADDR(EETOKEN), 0, sizeof(EETOKEN)); }

#define eEE_CHECKSIZE(DATA) struct FailOnEEPromExceess { int c[E2END-sizeof(DATA)]; };

#define PPRINT(TEXT) eEEPROM.showPgmString(PSTR(TEXT))

/*
 * Macros that define round robin integer arrays with user defined size;
 */
#define eEE_rrint7_t(NAME,COUNT)  s_rrint7  NAME[COUNT]
#define eEE_rrint15_t(NAME,COUNT) s_rrint15 NAME[COUNT]
#define eEE_rrint31_t(NAME,COUNT) s_rrint31 NAME[COUNT]

/*
 * Same for arbitrary user structures.
 */
#define eEE_rrstruct(NAME,COUNT,DEF) struct NAME { uint8_t current; DEF data; } NAME[COUNT]

/*
 * Macros to transparently init/read/write such round robin arrays.
 */
#define eEE_RRWRITE(NAME,VALUE) eEEPROM.rrWrite(NAME, sizeof(NAME)/sizeof(NAME[0]), VALUE)
#define eEE_RRREAD(NAME)        eEEPROM.rrRead( NAME, sizeof(NAME)/sizeof(NAME[0]))

#define eEE_RRWRITESTRUCT(NAME,SOURCE) eEEPROM.rrWriteStruct((s_rrstruct*)&(NAME), sizeof(NAME), sizeof(NAME)/sizeof(NAME[0]), &(SOURCE));
#define eEE_RRREADSTRUCT (NAME,TARGET) eEEPROM.rrReadStruct( (s_rrstruct*)&(NAME), sizeof(NAME), sizeof(NAME)/sizeof(NAME[0]),  &(TARGET));

// eEEPROM_h
#endif
