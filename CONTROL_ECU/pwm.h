/*
 * pwm.h
 *
 *  Created on: Oct 10, 2023
 *      Author: Hp
 */

#ifndef PWM_H_
#define PWM_H_
#include "std_types.h"

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Function responsible for initialize the PWM driver.
 */
void PWM_Timer0_Start(uint8 duty_cycle);


#endif /* PWM_H_ */
