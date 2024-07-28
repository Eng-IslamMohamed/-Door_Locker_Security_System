/*
 * dc.h
 *
 *  Created on: Oct 8, 2023
 *      Author: Hp
 */

#ifndef DC_H_
#define DC_H_

#include "std_types.h"
#include "gpio.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define MOTOR_PORT_ID       PORTB_ID
#define MOTOR_FPIN_ID       PIN0_ID
#define MOTOR_SPIN_ID		PIN1_ID
#define MOTOR_ENABLE_ID		PIN3_ID

typedef enum{
	CW,ACW,STOP
}DcMotor_State;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * Function responsible for initialize the DC Motor driver.
 */
void DcMotor_init(void);

/*
 * Description :
 * Function responsible for directing the DC Motor to a required direction in a desired speed.
 */
void DcMotor_Rotate(DcMotor_State state,uint8 speed);

#endif /* DC_H_ */
