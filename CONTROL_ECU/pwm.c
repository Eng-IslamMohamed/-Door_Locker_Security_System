/*
 * pwm.c
 *
 *  Created on: Oct 10, 2023
 *      Author: Hp
 */
#include "pwm.h"
#include "std_types.h"
#include "common_macros.h"
#include "gpio.h"
#include <avr/io.h>

void PWM_Timer0_Start(uint8 duty_cycle){

	// Configure PB3/OC0 as output pin --> pin where the PWM signal is generated from MC
	DDRB  = DDRB | (1<<PB3);
	/* Configure timer control register
		 * 1. Fast PWM mode FOC0=0
		 * 2. Fast PWM Mode WGM01=1 & WGM00=1
		 * 3. Clear OC0 when match occurs (non inverted mode) COM00=0 & COM01=1
		 * 4. clock = F_CPU/8 CS00=0 CS01=1 CS02=0
		 */
	TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<CS01);
	TCNT0 = 0; // Set Timer Initial Value to 0

	OCR0  = (uint8)(((float32)duty_cycle/100)*255);  // Set Compare Value


}
