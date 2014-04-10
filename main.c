#include <msp430.h> 
#include "main.h"
/*
 * main.c
 * Paul Hey C0320318
 * FanController - Version[0.1]
 *
 * [2014 - APR - 10]
 * 0.1 =>	Echoes back on
 * 			USART @ 128000 BAUD.
 *
 */

//Global Variables

struct{
	//TimerMode tm;
	//unsigned char TxBitCount;
	unsigned char TxBuffer;

}GV;


void main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	if (CALBC1_16MHZ == 0xFF) {	// If calibration constant erased
		while (1) {
		}                       // do not load, trap CPU!!
	}
	DCOCTL = 0;                	// Select lowest DCOx and MODx settings
	BCSCTL1 = CALBC1_16MHZ;		// Set range
	DCOCTL = CALDCO_16MHZ;  	// Set DCO step + modulation

	init();
	SetupUSART_A0();
	__bis_SR_register(LPM4_bits + GIE);

	//return 0;
}

void init(void){
	GV.TxBuffer = 0x00;

	P1DIR = PxDIR_ALL_OUT;
	P1OUT = PxOUT_ALL_OUT;
	P1SEL = TXD + RXD;
	P1SEL2 = TXD + RXD;

	P2DIR = PxDIR_ALL_OUT;
	P2OUT = PxOUT_ALL_OUT;

}

//Setup USART Hardware for 16MHz 128Kbaud
void SetupUSART_A0(void){
	UCA0CTL1 |= UCSSEL_2;
	UCA0BR0 = UCBR0_128Kbaud;//Lower Baud rate byte
	UCA0BR1 = 0x00;//Upper Baud rate byte
	UCA0MCTL = 0x00;//Modulation control Register
	UCA0CTL1 &= ~UCSWRST;
	IE2 |= UCA0RXIE;
}

//ISRs

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0_RX_ISR(void){
	while(!(IFG2&UCA0TXIFG));
	UCA0TXBUF = UCA0RXBUF;
}
