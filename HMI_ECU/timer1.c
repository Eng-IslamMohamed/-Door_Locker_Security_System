/*
 * timer1.c
 *
 *  Created on: Oct 31, 2023
 *      Author: Hp
 */

#include "timer1.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;

ISR(TIMER1_COMPA_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

ISR(TIMER1_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

void Timer1_init(const Timer1_ConfigType * Config_Ptr){

	TCNT1 = Config_Ptr -> initial_value;

	if((Config_Ptr -> mode) == NORMAL){
		TCCR1A |= (1<<FOC1A) |(1<<FOC1B);
		TCCR1B = (TCCR1B & 0xF8) | ((Config_Ptr -> prescaler));
		TIMSK |= (1<<TOIE1);
	}
	else if((Config_Ptr -> mode) == CTC){
		TCCR1A |= (1<<FOC1A) |(1<<FOC1B);
		TCCR1B = (TCCR1B & 0xE7) | (((Config_Ptr -> mode)&0xFC)<<1);
		TCCR1B = (TCCR1B & 0xF8) | ((Config_Ptr -> prescaler));
		OCR1A = Config_Ptr -> compare_value;
		TIMSK |= (1<<OCIE1A);
	}

}

void Timer1_deInit(void){
	TCNT1=0;
	OCR1A=0;
	TCCR1A=0;
	TCCR1B=0;
	TIMSK &= ~(1<<TOIE1) & ~(1<<OCIE1A);
}

void Timer1_setCallBack(void(*a_ptr)(void)){

	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;

}
