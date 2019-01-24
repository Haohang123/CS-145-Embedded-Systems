/*
 * Project 1.c
 *
 * Created: 1/16/2019 5:49:38 PM
 * Author : Jackson Tsoi
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "avr.h"

int main(void)
{
    /* Replace with your application code */
//     while (1) 
//     {
//     }
	DDRB = 0x01;
	for( ; ;) {
		if(!GET_BIT(PINB,1))
		{
			SET_BIT(PORTB,0);  /* LED ON*/
			wait_half_sec();
			CLR_BIT(PORTB,0);
			wait_half_sec();	
		}
	}

}

