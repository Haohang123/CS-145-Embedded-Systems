/*
 * avr.c
 *
 * Created: 1/30/2019 12:36:05 PM
 *  Author: Jackson Tsoi
 */ 

#include "avr.h"

void
avr_init(void)
{
	WDTCR = 15;
}

void
avr_wait(unsigned short msec)
{
	TCCR0 = 3;
	while (msec--) {
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001);
		SET_BIT(TIFR, TOV0);
		WDR();
		while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0;
}


void avr_wait2(unsigned short subsec)
{
	TCCR0 = 3;
	while (subsec--) {
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.00005);
		SET_BIT(TIFR, TOV0);
		WDR();
		while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0;
}