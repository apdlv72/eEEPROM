
#include <eEEPROM.h>

struct mydata 
{ 
  eEE_rrint7_t(myA,10);
  eEE_rrint15_t(myB,10);
} * EE = 0;

eEE_CHECKSIZE(*EE)

void setup()
{
    Serial.begin(9600);

    eEE_RRWRITE(EE->myA,  100);
    eEE_RRWRITE(EE->myB, 1000);
}

void loop()
{
    int    i;
    long   l;
  
    i = eEE_RRREAD(EE->uptime);
    l = eEE_RRREAD(EE->stats);

    Serial.println(i);
    Serial.println(l);

    delay(10*1000);
}
