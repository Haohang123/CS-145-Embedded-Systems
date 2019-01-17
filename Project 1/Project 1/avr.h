/*
 * avr.h
 *
 * Created: 1/16/2019 5:54:56 PM
 *  Author: Jackson Tsoi
 */ 

#ifndef _AVR_H
#define _AVR_H

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/io.h>

#define XTAL_FRQ 8000000lu

#define SET_BIT(p,i) ((p) |=  (1 << (i)))
#define CLR_BIT(p,i) ((p) &= ~(1 << (i)))
#define GET_BIT(p,i) ((p) &   (1 << (i)))

void avr_wait(unsigned short msec);

#endif /* _AVR_H */