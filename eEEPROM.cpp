#include "Arduino.h"
#include "eEEPROM.h"

#include <avr/eeprom.h>

uint8_t eEEPROMClass::read(int addr)
{
	return eeprom_read_byte((unsigned char *) addr);
}

void eEEPROMClass::write(int addr, uint8_t value)
{
	// write only if there is no change to avoid writy cycles
	uint8_t old = read(addr);
	if (old!=value)
	{
		eeprom_write_byte((unsigned char *) addr, value);
	}
}

void eEEPROMClass::doWrite(int addr, uint8_t value)
{
	eeprom_write_byte((unsigned char *) addr, value);
}

uint16_t eEEPROMClass::readWord(int addr)
{
	uint16_t w;
	readData(addr, &w, sizeof(w));
	return w;
}

void eEEPROMClass::writeLong(int addr, uint32_t l)
{
	writeData(addr, &l, sizeof(l));
}

uint32_t eEEPROMClass::readLong(int addr)
{
	uint32_t l;
	readData(addr, &l, sizeof(l));
	return l;
}

void eEEPROMClass::writeWord(int addr, uint16_t w)
{
	writeData(addr, &w, sizeof(w));
}

float eEEPROMClass::readFloat(int addr)
{
	float f;
	readData(addr, &f, sizeof(f));
	return f;
}

void eEEPROMClass::writeFloat(int addr, float f)
{
	writeData(addr, &f, sizeof(f));
}

double eEEPROMClass::readDouble(int addr)
{
	double d;
	readData(addr, &d, sizeof(d));
	return d;
}

void eEEPROMClass::writeDouble(int addr, double d)
{
	writeData(addr, &d, sizeof(d));
}

void eEEPROMClass::readData(int addr, void * buf, int len)
{
	for (uint8_t * b=(uint8_t*)buf; len>0; len--, addr++, b++)
	{
		*b = read(addr);
	}	
}

void eEEPROMClass::writeData(int addr, const void * buf, int len)
{
	for (const uint8_t * b=(uint8_t*)buf; len>0; len--, addr++, b++)
	{
		write(addr, *b);
	}	
}

void eEEPROMClass::memFill(int addr, uint8_t data, uint16_t len)
{
	for (; len>0; len--, addr++)
	{
		write(addr, data);
	}
}

void eEEPROMClass::showPgmString (PGM_P s)
{
	char c;
	while ((c = pgm_read_byte(s++)) != 0)
	{
		Serial.print(c);
	}
}

eEEPROMClass eEEPROM;
