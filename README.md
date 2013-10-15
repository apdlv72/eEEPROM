eEEPROM
=======

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
     
