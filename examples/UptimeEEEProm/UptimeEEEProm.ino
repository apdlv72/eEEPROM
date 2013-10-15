/*
This sample demonstrates how to use the round robin integer arrays of the eEEPROM
library to implement counters that will track total number of operation for a device
as well as record how often the device was started.

Using round robin arrays that spread the write cycles over 40 resp. 20 bytes,
will avoid that the max. write cycles to a single cell is exceeded too soon. 
Allowing even more EEProm cells to be "wasted" for this purpose, the time ranges
explained below may even be increased. 
*/

#include <eEEPROM.h>

// I am from cologne ;)
const static uint16_t MAGIC = 4711;

// everythin that needs to go to EEProm is inside the follwoing structure:
struct mydata 
{ 
  // magic to detect if EEProm was initialized for the first time
  uint16_t magic;
  // number of times this device was started
  eEE_rrint15_t(starts, 10);
  // uptime in hours spread over 10*4 bytes can store up to 122k years (2^30-1)/24,
  // however eeprom write cycles will be excessed in approx. 57 years (100k*10/2)/24/365)
  // when written every hour
  eEE_rrint31_t(uptime, 10); 
} 
* EE = 0;

// macro checks whether we might exceeded the EEProms capacity
eEE_CHECKSIZE(*EE)

uint16_t uptime_last = 0;
uint16_t uptime      = 0;
uint32_t starts      = 0;


void setup()
{
  Serial.begin(9600);
  
  uint16_t magic = eEEPROM.readWord(EE->magic);  
  if (MAGIC==magic)
  {
    Serial.println("setup: magic found"); 
    uptime = uptime_last = eEE_RRREAD(EE->uptime);
    starts = eEE_RRREAD(EE->starts);
    starts++;    
  }
  else
  {
    Serial.println("setup: no magic found, 1st time init"); 
    eEEPROM.writeWord(EE->magic, MAGIC);
    uptime_last = uptime = 0;
    starts = 1;
  }

  eEE_RRWRITE(EE->starts, starts);
  
  Serial.print("setup: starts: "); Serial.println(starts);
  Serial.print("setup: uptime: "); Serial.println(uptime);
}


void loop()
{
  // write uptime (in hours) whenever it changes
  uptime = millis()/(60*60*1000);
  if (uptime_last!=uptime)
  {
    eEE_RRWRITE(EE->uptime, uptime);
    uptime_last = uptime;
  }

  // do something usefull here like blinking LEDs
  
  Serial.print("loop: uptime: "); Serial.print(uptime); Serial.println(" h");  
  delay(10*1000);
}
