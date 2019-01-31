/*
 * avr.c
 *
 * Created: 1/16/2019 5:55:45 PM
 *  Author: Jackson Tsoi & David Pham
 */ 

#include "avr.h"

void
wait_avr(unsigned short msec)
{
	TCCR0 = 3;
	while (msec--) {
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001);
		SET_BIT(TIFR, TOV0);
		while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0;
}

void wait_half_sec() {
	volatile int i;
	unsigned long n = 50000;
	for(i=0; i < n; ++i) {
		/* nothing 
	}
}

