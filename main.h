/*
 * main.h
 *
 *  Created on: Apr 10, 2014
 *      Author: C0320318
 */

#ifndef MAIN_H_
#define MAIN_H_

#define PxDIR_ALL_OUT 0xff
#define PxOUT_ALL_OUT 0x00

#define UCOS16_128Kbaud 0
#define UCBR0_128Kbaud 125
#define UCBRS0_128Kbaud 0
#define UCBRF0_128Kbaud 0

#define LED1 BIT0
#define LED2 BIT6

#define LEDS_DIR P1DIR
#define LEDS_OUT P1OUT

#define TXD BIT1
#define RXD BIT2

/*
typedef enum{
	PWM_MODE = 0,
	UART_MODE
}TimerMode;*/

void init(void);
void SetupUSART_A0(void);

#endif /* MAIN_H_ */
