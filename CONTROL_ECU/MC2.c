/*
 * MC2.c
 *
 *  Created on: Oct 29, 2023
 *      Author: Islam Mohamed Hossam
 */
//Including Files
#include "uart.h"
#include "external_eeprom.h"
#include "twi.h"
#include "timer1.h"
#include "std_types.h"
#include <util/delay.h>
#include "dc.h"
#include "lcd.h"
#include "buzzer.h"
#include "common_macros.h"
#include <avr/io.h>

//Definitions
#define PASS_SIZE 5
#define PASS_CONFIRMED 5
#define SUCCESS 1
#define ERROR 0
#define COMPARE_SUCCESS 1
#define COMPARE_FAILED 0
#define MAX_SECURITY_BREACH 3
#define SECURITY_BREACH 0
#define NO_SECURITY_BREACH 1
#define DOOR_OPENING 1
#define DOOR_CLOSING 0
#define NUMBER_OVERFLOWS_PER_SECOND 122

//Global Variables
static uint8 key;
uint8 i,p1[PASS_SIZE],p2[PASS_SIZE],bool,g_readPasswordArray[5],g_readPassword,flag=0,g_equalPressed=0;
uint8 g_passFailed,g_tick=0,g_count=0,g_breachCount=0,g_seconds=0;
uint16 g_address=0x0311;


/*Function used to receive the first pass from the user
and displaying every button the user pressed and doesn't leave the function
before leaving the function
 */
void receiveFirstPass(void){
	uint8 pass1;
	LCD_clearScreen();

	/*
	 * receiving first pass byte by byte and saving every byte received from MC1
	 * in a variable called pass1 then saving the value of pass1 in the array
	 */
	for(i=0;i<PASS_SIZE;i++){
		pass1=UART_recieveByte();
		_delay_ms(10);
		p1[i]=pass1;
		LCD_intgerToString(pass1);
	}
	/*
	 * making sure the user pressed = button before proceeding in the code
	 */
	g_equalPressed=UART_recieveByte();
}

void receiveSecondPass(void){

	uint8 pass2;
	LCD_clearScreen();

	/*
	 * receiving second pass byte by byte and saving every byte received from MC1
	 * in a variable called pass2 then saving the value of pass2 in the array
	 */
	for(i=0;i<PASS_SIZE;i++){
		pass2=UART_recieveByte();
		_delay_ms(10);
		p2[i]=pass2;
		LCD_intgerToString(pass2);
	}
	/*
	 * making sure the user pressed = button before proceeding in the code
	 */
	g_equalPressed=UART_recieveByte();

}

/*
 * function used to verify that the two entered password from the user at the beginning
 * are identical and sends success if they are identical, ERROR if not
 */
void authenticatePass(void){
	flag=0;

	//verify that the two entered password from the user at the beginning are identical
	for(i=0;i<PASS_SIZE;i++){
		if(p1[i]==p2[i]){
			flag++;
		}
	}
	//sending Success if passwords are identical
	if(flag==PASS_CONFIRMED){
		LCD_clearScreen();
		bool = SUCCESS;
		UART_sendByte(bool);
		_delay_ms(10);
	}
	//sending Error if passwords are not identical
	else{
		LCD_clearScreen();
		bool = ERROR;
		UART_sendByte(bool);
		_delay_ms(10);
	}
}

/*
 * function used to save password in the EEPROM Memory byte by byte
 */
void EEPROM_savePass(void){
	LCD_clearScreen();
	for(i=0;i<PASS_SIZE;i++){
		//writing the password in EEPROM Memory byte by byte
		EEPROM_writeByte(g_address + i, p1[i]);
		_delay_ms(10);

	}
}

/*
 * function used to verify that the confirmation pass is equal to the saved password in EEPROM
 * flag is used to increment when a byte is equal to its equivalent in the saved pass array
 * when flag reaches to 5 then the password is verified and then Comparing is success,
 * if not Comparing fails
 */
void EEPROM_comparePass(void){
	flag=0;
	LCD_clearScreen();
	LCD_displayString("savedPass=");
	for(i=0;i< PASS_SIZE;i++){
		EEPROM_readByte(g_address + i,&g_readPassword);
		_delay_ms(10);
		LCD_intgerToString(g_readPassword);
		g_readPasswordArray[i]=g_readPassword;
	}

	for(i=0;i<PASS_SIZE;i++){
		if(p1[i]==g_readPasswordArray[i]){
			flag++;
		}
	}

	if(flag==PASS_CONFIRMED){
		bool = COMPARE_SUCCESS;

	}
	else{
		bool = COMPARE_FAILED;
	}


}


/*
 * this is the timer call back function in case of compare mode
 */
void APP_tickCounter(void){
	g_tick++;
}

/*
 * security breach is a function used to take another password from the user
 * to verify that he is the actual user of the system ,this pass is entered
 * after selecting '+' or '-'
 *
 * it compares the pass with the one saved in the EEPROM Memory
 *
 * this function offers 3 times of password trial if password is confirmed with the saved one
 * then a global variable g_passFailed is assigned with the value of NO_SECURITYBREACH
 *
 * if not the a global variable g_breach count increments by 1 and if it reaches 3 then
 * we will consider it as a security breach
 *  and g_passFailed will take the value of SECURITY_BREACH
 */
void securityBreach(void){
	while(1){
		LCD_clearScreen();
		receiveFirstPass();
		EEPROM_comparePass();

		//sending the value of EEPROM comparison
		UART_sendByte(bool);
		_delay_ms(10);

		if(bool == COMPARE_SUCCESS){
			g_passFailed=NO_SECURITY_BREACH;
			break;
		}
		g_breachCount++;
		if(g_breachCount==3){
			break;
		}
		g_passFailed=SECURITY_BREACH;

	}
}

int main(void){
	SET_BIT(SREG,7);//enabling global interrupt bit "I-Bit"
	UART_ConfigType UART_Configurations = {ASYNCHRONOUS,EIGHT_BIT,EVEN_PARITY,ONE_BIT,9600}; //configures the UART dynamically
	TWI_ConfigType TWI_Configurations ={0x02,0x02}; //configures I2C Dynamically
	Timer1_ConfigType TIMER1_Configurations = {0,31249,F_CPU_256,CTC};//configures TIMER1 Dynamically

	//initializing used drivers
	Timer1_init(&TIMER1_Configurations);
	LCD_init();
	TWI_init(&TWI_Configurations);
	UART_init(&UART_Configurations);
	DcMotor_init();
	Buzzer_init();

	//sending address of the call back function
	Timer1_setCallBack(APP_tickCounter);

	while(1){
		//initial values
		g_passFailed=0;
		g_breachCount=0;

		LCD_clearScreen();
		receiveFirstPass();
		receiveSecondPass();
		authenticatePass();

		/* if the compare was success then proceed the code
		 * if not then ask the user for the passwords again
		 */
		if(bool==SUCCESS){
			EEPROM_savePass();//saving passwords in EEPROM Memory after authentication

			while(1){
				key=UART_recieveByte();//recieving + or -
				_delay_ms(10);
				g_count=0;
				if(key == '-'){

					while(1){
						//checking for security breach
						securityBreach();


						if(g_passFailed==SECURITY_BREACH &&	g_breachCount==3){
							//buzzer on
							Buzzer_on();

							g_tick=0;
							while(g_tick<60);
							g_tick=0;
							Buzzer_off();
							break;
						}
						else if(g_passFailed==NO_SECURITY_BREACH){
							break;
						}

					}

				}

				else if(key == '+'){

					while(1){
						//checking for security breach
						securityBreach();


						if(g_passFailed==SECURITY_BREACH &&	g_breachCount==3){
							//buzzer on
							Buzzer_on();
							g_tick=0;
							while(g_tick<60);
							g_tick=0;
							Buzzer_off();
							break;
						}
						else if(g_passFailed==NO_SECURITY_BREACH){
							g_tick=0;
							DcMotor_Rotate(CW,50);
							while(g_tick<15);
							g_tick=0;
							DcMotor_Rotate(STOP,0);
							while(g_tick<3);
							g_tick=0;
							DcMotor_Rotate(ACW,50);
							while(g_tick<15);
							DcMotor_Rotate(STOP,0);
							g_tick=0;
							break;
						}
					}
				}
				if(g_passFailed==NO_SECURITY_BREACH && key=='-'){
					break;
				}

			}

		}

	}
}
