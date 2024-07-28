/*
 * dc.c
 *
 *  Created on: Oct 8, 2023
 *      Author: Hp
 */

#include "dc.h"
#include "gpio.h"
#include "std_types.h"
#include "common_macros.h"
#include "pwm.h"



void DcMotor_init(void){
	/*Setup the direction for the two motor pins through gpio driver*/
	GPIO_setupPinDirection(MOTOR_PORT_ID, MOTOR_FPIN_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(MOTOR_PORT_ID, MOTOR_SPIN_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(MOTOR_PORT_ID, MOTOR_ENABLE_ID, PIN_OUTPUT);
	/*stopping the DC Motor at the beginning through gpio driver*/
	GPIO_writePin(MOTOR_PORT_ID, MOTOR_FPIN_ID, LOGIC_LOW);
	GPIO_writePin(MOTOR_PORT_ID, MOTOR_SPIN_ID, LOGIC_LOW);
}

void DcMotor_Rotate(DcMotor_State state,uint8 speed){
	switch(state){
	case CW:
		/*
		 *Rotating the dc motor CW
		 */
		GPIO_writePin(MOTOR_PORT_ID, MOTOR_FPIN_ID, LOGIC_HIGH);
		GPIO_writePin(MOTOR_PORT_ID, MOTOR_SPIN_ID, LOGIC_LOW);
		break;
	case ACW:
		/*
		 *Rotating the dc motor ACW
		 */
		GPIO_writePin(MOTOR_PORT_ID, MOTOR_FPIN_ID, LOGIC_LOW);
		GPIO_writePin(MOTOR_PORT_ID, MOTOR_SPIN_ID, LOGIC_HIGH);
		break;
	case STOP:
		/*
		 *Stopping the dc motor
		*/
		GPIO_writePin(MOTOR_PORT_ID, MOTOR_FPIN_ID, LOGIC_LOW);
		GPIO_writePin(MOTOR_PORT_ID, MOTOR_SPIN_ID, LOGIC_LOW);
		break;
	}
	PWM_Timer0_Start(speed);
}
