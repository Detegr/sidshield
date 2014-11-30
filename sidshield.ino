#include <SPI.h>
#include <SD.h>

#define A4 14
#define A3 15
#define A2 16
#define A1 17
#define A0 19

#define D0 4
#define D1 3
#define D2 2
#define D3 1
#define D4 0
#define D5 5
#define D6 6
#define D7 7

#define RESETPIN 18
#define CLOCKPIN 9
#define CSPIN 8

void setup()
{
	pinMode(CLOCKPIN, OUTPUT);
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1  = 0;
	OCR1A = 7;   // toggle after counting to 8
	TCCR1A |= (1 << COM1A0);   // Toggle OC1A on Compare Match.
	TCCR1B |= (1 << WGM12);    // CTC mode
	TCCR1B |= (1 << CS10);     // clock on, no pre-scaler

	for(int i=0; i<=10; ++i)
	{
		if(i==CLOCKPIN) continue;
		pinMode(i, OUTPUT);
	}
	for(int i=14; i<=19; ++i)
	{
		pinMode(i, OUTPUT);
	}
	digitalWrite(CSPIN, HIGH);
	resetSid();
}

inline void parseRegister(uint8_t val)
{
	digitalWrite(A0, (val&0x1));
	digitalWrite(A1, (val&0x2));
	digitalWrite(A2, (val&0x4));
	digitalWrite(A3, (val&0x8));
	digitalWrite(A4, (val&0x10));
}

inline void parseValue(uint8_t val)
{
	digitalWrite(D0, (val & 0x1));
	digitalWrite(D1, (val & 0x2));
	digitalWrite(D2, (val & 0x4));
	digitalWrite(D3, (val & 0x8));
	digitalWrite(D4, (val & 0x10));
	digitalWrite(D5, (val & 0x20));
	digitalWrite(D6, (val & 0x40));
	digitalWrite(D7, (val & 0x80));
}

inline void sidWrite(uint8_t reg, uint8_t val)
{
	parseRegister(reg);
	parseValue(val);
	transfer();
}

inline void transfer(void)
{
	digitalWrite(CSPIN, LOW);
	delayMicroseconds(2);
	digitalWrite(CSPIN, HIGH);
}

inline void resetSid(void)
{
	digitalWrite(RESETPIN, LOW);
	delayMicroseconds(1000);
	digitalWrite(RESETPIN, HIGH);
}

void loop()
{
	if(!SD.begin(10)) return;
	else
	{
		File sid=SD.open("CBR.SDU");
		if(sid)
		{
			int reg, val;
			unsigned long t=millis();
			while(sid.available())
			{
				reg=sid.read();
				if(reg==0xFF)
				{// New frame
					unsigned long t2=millis();
					delay(20 - (t2-t));
					t=millis();
					continue;
				}
				val=sid.read();
				if(reg<0 || val<0) break;
				sidWrite(reg,val);
			}
			sid.close();
		}
		else
		{
			while(1)
			{
				delay(1000);
			}
		}
	}
}
