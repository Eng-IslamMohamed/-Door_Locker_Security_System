/*
 * main.c
 *
 *  Created on: Oct 29, 2023
 *      Author: Islam Mohamed Hossam
 */
//Including Files
#include "lcd.h"
#include "uart.h"
#include "gpio.h"
#include "keypad.h"
#include "uart.h"
#include "timer1.h"
#include "common_macros.h"
#include <avr/io.h>
#include "std_types.h"
#include <util/delay.h>

//Definitions
#define PASS_SIZE 5
#define SUCCESS 1
#define ERROR 0
#define DOOR_OPENING 1
#define DOOR_CLOSING 0
#define SECURITY_BREACH 0
#define NO_SECURITY_BREACH 1
#define COMPARE_SUCCESS 1
#define COMPARE_FAILED 0
#define MAX_SECURITY_BREACH 3
#define NUMBER_OVERFLOWS_PER_SECOND 122

//Global Variables
uint8 i,bool,key,g_passFailed,g_breach,g_count=0,g_breachCount=0,g_tick=0,g_seconds;

/*Function used to send the first pass from the user
and displaying '*' after every button the user pressed*/
void sendFirstPass(void){
	LCD_clearScreen();
	LCD_displayString("plz enter pass:");
	LCD_moveCursor(1,0);

	for(i=0;i<PASS_SIZE;i++){


		key=KEYPAD_getPressedKey();
		//Checking if the pressed key is a number from 0 to 9
		if((key<0) || (key>9)){
			i--;
			continue;
		}
		/*
		 * sending first pass byte by byte
		 */
		LCD_displayCharacter('*');
		UART_sendByte(key);

		_delay_ms(500);
	}

	/*
	 * making sure the user pressed = button before proceeding in the code
	 */
	while(key != '='){
		key=KEYPAD_getPressedKey();
	}
	UART_sendByte(key);//sending '='
	_delay_ms(500);
}

/*Function used to send the second pass from the user
and displaying '*' after every button the user pressed*/
void sendSecondPass(void){

	LCD_clearScreen();
	LCD_displayString("plz re-enter the");
	LCD_moveCursor(1,0);
	LCD_displayString("same pass: ");


	for(i=0;i<PASS_SIZE;i++){

		key=KEYPAD_getPressedKey();
		//Checking if the pressed key is a number from 0 to 9
		if((key<0) || (key>9)){
			i--;
			continue;
		}
		/*
		 * sending second pass byte by byte
		 */
		LCD_displayCharacter('*');

		UART_sendByte(key);
		_delay_ms(500);
	}
	/*
	 * making sure the user pressed = button before proceeding in the code
	 */
	while(key != '='){
		key=KEYPAD_getPressedKey();
	}
	UART_sendByte(key);//sending '='
	_delay_ms(500);
}

/*
 * security breach is a function used to take another password from the user
 * to verify that he is the actual user of the system ,this pass is entered
 * after selecting '+' or '-'
 *
 * this function receives the value of the comparing pass with the saved pass
 * in the EEPROM Memory from MC2
 *
 * this function offers 3 times of password trial if password is confirmed with the saved one
 * then a global variable g_passFailed is assigned with the value of NO_SECURITY_BREACH
 *
 * if not the a global variable g_breach count increments by 1 and if it reaches 3 then
 * we will consider it as a security breach
 *  and g_passFailed will take the value of SECURITY_BREACH
 */

void securityBreach(void){
	while(1){
		sendFirstPass();

		g_passFailed=UART_recieveByte();//receiving EEPROM COMPARE Value
		_delay_ms(10);


		if(g_passFailed == NO_SECURITY_BREACH){
			break;
		}
		g_breachCount++;
		if(g_breachCount==3){
			break;
		}
		g_passFailed=SECURITY_BREACH;

		LCD_clearScreen();
	}
}

/*
 * this is the timer call back function in case of compare mode
 */
void APP_tickCounter(void){
	g_tick++;//every second is equivalent to one tick 1sec=1tick
}


int main(void){
	SET_BIT(SREG,7);//enabling global interrupt bit "I-Bit"
	UART_ConfigType UART_Configurations = {ASYNCHRONOUS,EIGHT_BIT,EVEN_PARITY,ONE_BIT,9600}; //configures the UART dynamically
	Timer1_ConfigType TIMER1_Configurations = {0,31249,F_CPU_256,CTC};//configures TIMER1 Dynamically

	//initializing used drivers
	Timer1_init(&TIMER1_Configurations);
	Timer1_setCallBack(APP_tickCounter);//sending address of the call back function
	LCD_init();
	UART_init(&UART_Configurations);


	while(1){
		//initial values
		g_passFailed=0;
		g_breachCount=0;

		//sending first pass and second pass
		sendFirstPass();

		sendSecondPass();

		//receiving value of compare between first pass and second pass
		bool=UART_recieveByte();
		_delay_ms(10);

		/* if the compare was success then proceed the code
		 * if not then ask the user for the passwords again
		 */
		if(bool == SUCCESS){

			while(1){
				LCD_clearScreen();
				LCD_displayString("+ : Open Door");
				LCD_moveCursor(1,0);
				LCD_displayString("- : Change Pass");

				//making sure the pressed button was '+' or '-'
				while(1){
					key=KEYPAD_getPressedKey();
					if(key == '+' || key=='-'){
						break;
					}
				}
				//sending + or -
				UART_sendByte(key);
				_delay_ms(10);
				g_count=0;
				if(key == '-'){
					while(1){
						//checking if security breach occurred
						securityBreach();

						if(g_passFailed==SECURITY_BREACH && g_breachCount == 3){
							LCD_clearScreen();
							LCD_displayString("SECURITY BREACH!");
							g_tick=0;
							while(g_tick<60);
							g_tick=0;
							break;
						}
						else if(g_passFailed==NO_SECURITY_BREACH){
							break;
						}
					}
				}

				else if(key == '+'){
					while(1){
						//checking if security breach occurred
						securityBreach();

						if(g_passFailed==SECURITY_BREACH && g_breachCount == 3){
							LCD_clearScreen();
							LCD_displayString("SECURITY BREACH!");
							g_tick=0;
							while(g_tick<60);
							g_tick=0;
							break;
						}
						else if(g_passFailed==NO_SECURITY_BREACH){
							g_tick=0;
							LCD_clearScreen();
							LCD_displayString("Door Unlocking");
							while(g_tick<15);
							g_tick=0;
							LCD_clearScreen();
							LCD_displayString("Door Unlocked");
							while(g_tick<3);
							g_tick=0;
							LCD_clearScreen();
							LCD_displayString("Door is Locking");
							while(g_tick<15);
							g_tick=0;
							break;
						}
					}

				}
				if(g_passFailed == NO_SECURITY_BREACH && key=='-'){
					break;
				}
			}
		}
	}
}
