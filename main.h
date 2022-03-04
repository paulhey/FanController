/*
 * main.h
 *
 *  Created on: Apr 10, 2014
 *      Author: C0320318
 */

#ifndef MAIN_H_
#define MAIN_H_

//#include <mspmathlib.h>

#define PxDIR_ALL_OUT 0xff
#define PxOUT_ALL_OUT 0x00

#define UCOS16_128Kbaud 0
#define UCBR0_128Kbaud 125
#define UCBRS0_128Kbaud 0
#define UCBRF0_128Kbaud 0

#define LED1 BIT0
#define LED2 BIT6

#define MOSFET BIT3
#define TIMER0_A0_MAX 200  // For ~0.01ms @ 16MHz
#define TIMER0_A1_MAX 80   // For 50% DCY
#define TIMER0_A1_STEP 20
#define TIMER1_A0_MAX 16000  // For ~1ms
#define MS_COUNTER_MAX 1000  // For ~1s @ 16MHz
//#define ASCII_OFFSET 0x30
#define TS_MASK 0x000f
#define TX_TEMP_OFFSET 3
#define DEGREESC_OFFSET 9
#define THRESHOLD_OFFSET 21
#define PWM_DUTY_LEVEL_OFFSET 34
#define PWM_RAW_VALUE_OFFSET 44
#define MODE_OFFSET 55
#define SLOPE_HI 180
#define SLOPE_RATE 4
#define TARGET_TEMP (int)(25)
#define MAX_TEMP (int)(100)

#define LEDS_DIR P1DIR
#define LEDS_OUT P1OUT

#define TXD BIT1
#define RXD BIT2

#define ADC10_REF_SETTLE_TIME 16000  // 1us @ 16MHz?

typedef enum {
  _AUTO_MODE = 'a',
  _MANUAL_MODE = 'm',
  _SETUP_MODE = 's',
  _DEBUG_MODE = 'd',
  _SETUP_TEN = 't',
  _SETUP_ONE = 'o',
  _SETUP_ENTER = 'e'
} SystemMode;

void init(void);
void SetupUSART_A0(void);
void SetupTIMER0_A(void);
void SetupTIMER1_A(void);
void SetupADC10(void);
void ConvertRawToTemp(unsigned int my_raw, int* my_temp);
void TransmitGVTxString(const char temp[], unsigned char length);  // TODO: HERE

#endif /* MAIN_H_ */
