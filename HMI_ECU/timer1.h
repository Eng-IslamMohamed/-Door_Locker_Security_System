/*
 * timer1.h
 *
 *  Created on: Oct 31, 2023
 *      Author: Hp
 */

#ifndef TIMER1_H_
#define TIMER1_H_

#include "std_types.h"

typedef enum{
	NO_CLOCK,F_CPU_CLOCK,F_CPU_8,F_CPU_64,F_CPU_256,F_CPU_1024,FALLING_EDGE,RAISING_EDGE
}Timer1_Prescaler;
typedef enum{
	NORMAL,CTC=4
}Timer1_Mode;

typedef struct {
uint16 initial_value;
uint16 compare_value; // it will be used in compare mode only.
Timer1_Prescaler prescaler;
Timer1_Mode mode;
}Timer1_ConfigType;




void Timer1_init(const Timer1_ConfigType * Config_Ptr);
void Timer1_deInit(void);
void Timer1_setCallBack(void(*a_ptr)(void));


#endif /* TIMER1_H_ */
