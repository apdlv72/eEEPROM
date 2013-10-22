eEEPROM
=======

Arduino library to ease the usage of the uC's EEPROM.
 
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

To make these macros macros work, you must define a structure that wraps everything you
need to store in EEProm as a pointer to address 0.

	struct mydata
	{ 
		int  a; 
		long b; 
	} * EE = 0;
	
	eEE_CHECKSIZE(*EE);

The macro eEE_CHECKSIZE will fail with an error message, when the total size of the
structure exceeds the maximum EEProm capacity. The error message however is a bit cryptic.
It says something like "overflow in array dimension" - but that is much better than running
into funny problems at runtime anyway.

Reading and writing the tokens in EEProm then will become merely a childs play without the 
hassle of computing addreses and data sizes:

	eEE_WRITEN(EE->b, 1234L);	
	int i = eEE_READN(EE->a);

Sometimes you need to store values quite often and will run into the 100k write cycles limits
in a couple of hours or even minutes. To circumvent this, you would need to distribute the writes
to single EEProm cell equally over a bunch of bytes. This is where the round robin macros come in.
With the following definition (inside the EE struct depicted above):

	struct mydata 
	{ 
		int  a; 
		long b; 
		eEE_rrint7_t(myRRI,200); 
	} * EE = 0;

you creat an array of 200 8-bit integers where the MSB is used as a flag which one is the current one.
To access the current value, the macros 

	eEE_WRITERRI(EE->myRRI,  value);
	int8_t value = eEE_READRRI(EE->myA);  

will write resp. read the value stored and extend the life time of your EEProm by a factor or 200/2
thus at least 100k*200 = 20mio write operations or even more, because the library still will take care
not to overwrite the target with the same value.
Besides the 7-bit integer, there are version for 15 and 31 bit as well.

The same approach can be extended to more complex data types than just integer values.
For this to work, you wrap your data in a struct as you please and use the macro as follows:

	typedef struct { int a; int b; } mystruct;

	struct mydata 
	{ 
		int  a; 
		// ...
		eEE_rrstruct(myStruct,10,mystruct);
	} * EE = 0;
  
Reading and writing the structs is provided by the macros

	mystruct toBeSaved = { a:111, b:222 };
	eEE_WRITERRS(toBeSaved, EE->myStruct);

	mystruct readFromEE;
	eEE_READRRS(EE->myStruct, readFromEE);
	
In this case, because a comparison might be quite time consuming for large structs, no check is 
performed if current and future values do match, but new values written always.
(however, in terms of single bytes, the checks will still be performed in the next array entry
that is about to become the current one).

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
     
Note that the PPRINT macro will cause many compiler warnings regarding non-initialized data.
You safely ignore this since it is due to a gcc bug which probably might get fixed in the future.
