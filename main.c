#include <msp430.h> 
#include "main.h"
#include "sample.h"
/*
 * main.c
 * Paul Hey C0320318
 * FanController - Version[0.3]
 *
 * TODO:	[X] Lower PWM frequency to 25KHz. Small fan is too noisy, even at 50KHz.
 * TODO:	[ ] Read Temp. Data every ms? (Setup Timer1_A0 for this, Toggle LED2)
 * TODO:	[ ] Print Temp. Data out every 1s over UART
 *
 * [2014 - APR - 13]
 * 0.3.1 =>	[?] Added ADC10 initialization code, as well as added
 * 				"universal" sample code (sample.c/.h)
 * 				This is untested code.
 * TODO:	[?] Do 8 or 4 point rolling average (is setup for 8 point)
 *
 * [2014 - APR - 12]
 * 0.3 =>	[u] Respond to single character commands over UART
 * 				Responds to 'u' with "Hello World"
 *
 * [2014 - APR - 10]
 * 0.2 =>	[W] PWM's LED1 and MOSFET 50% Duty Cycle @100KHz
 * 0.1 =>	[W] Echoes back on UART @ 128000 BAUD.
 */

//Global Variables
const char OutputString[] = { "Hello World\r\n" };
const char EchoString[] = {
		"Fan Controller Module:\r\nt => Current Temperature\r\nu => Hello\r\n" };

struct {

	unsigned char TxBuffer;
	unsigned int ms_Counter;	//Keep track of current ms count
	unsigned int Tx_i;
	ADCSampleData Temperature;

} GV;

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
	SetupADC10();
	SetupUSART_A0();
	SetupTIMER0_A0();
	__bis_SR_register(LPM0_bits + GIE);

	//return 0;
}

void init(void) {
	GV.TxBuffer = 0x00;
	GV.ms_Counter = 0x0000;
	GV.Tx_i = 0;

	P1DIR = PxDIR_ALL_OUT;
	P1OUT = PxOUT_ALL_OUT;
	P1SEL = TXD + RXD;
	P1SEL2 = TXD + RXD;

	P2DIR = PxDIR_ALL_OUT;
	P2OUT = PxOUT_ALL_OUT;
	P3DIR = PxDIR_ALL_OUT;
	P3OUT = PxOUT_ALL_OUT;

}

//Setup USART Hardware for 16MHz 128Kbaud
void SetupUSART_A0(void) {
	UCA0CTL1 |= UCSSEL_2;
	UCA0BR0 = UCBR0_128Kbaud;  	//Lower Baud rate byte
	UCA0BR1 = 0x00;  	//Upper Baud rate byte
	UCA0MCTL = 0x00;  	//Modulation control Register
	UCA0CTL1 &= ~UCSWRST;
	IE2 |= UCA0RXIE;
}

void SetupTIMER0_A0(void) {
	TA0CCR0 = TIMER0_A0_MAX;
	TA0CCR1 = TIMER0_A0_MAX >> 1;  	//For 50% Duty Cycle
	TA0CCTL0 |= CCIE;
	TA0CCTL1 |= CCIE;
	TA0CTL = TASSEL_2 + MC_1 + TACLR;  	//SMCLK, UP, CLEAR
}

//Setup the ADC10 for temperature conversion.
//V TEMP =0.00355(TEMP C )+0.986
//.: TEMP C = (V TEMP - 0.986)/0.00355
//[?](=) TEMP C = (V TEMP - 0.986) * 281.69[/?]
//Since ADC range is 10 bits (0x0000 to 0x03FF) (0.0V to 3V?)
//(=) TEMP C = (V TEMP
void SetupADC10(void) {
	ADC10CTL1 = INCH_10 + ADC10DIV_3;  	//Temp Sensor, ADC10CLK/4
	ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON + ADC10IE; //TODO:???
	__delay_cycles(ADC10_REF_SETTLE_TIME);
	ADC10CTL0 |= ENC + ADC10SC;
	//__bis_SR_register(CPUOFF + GIE);//TODO:???
	GV.Temperature.calibrated = ADC10MEM;//Initialize samples array
	InitializeSamples(&GV.Temperature);
}

//ISRs

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0_TX_ISR(void) {
	UCA0TXBUF = OutputString[GV.Tx_i++]; // TX next character
	if (GV.Tx_i == sizeof OutputString - 1) { // TX over?
		IE2 &= ~UCA0TXIE; // Disable USCI_A0 TX interrupt
	}
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0_RX_ISR(void) {
	//while(!(IFG2&UCA0TXIFG));
	//UCA0TXBUF = UCA0RXBUF;
	if (UCA0RXBUF == 'u') {	// 'u' received?
		GV.Tx_i = 0;
		IE2 |= UCA0TXIE;	// Enable USCI_A0 TX interrupt
		UCA0TXBUF = OutputString[GV.Tx_i++];

	}
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void) {
	//GV.ms_Counter++;
	//if(GV.ms_Counter==MS_COUNTER_MAX){//Toggle every second

	P1OUT |= LED1;
	P2OUT |= MOSFET;
	//	GV.ms_Counter = 0;//Reset Counter
	//}

}

/* The TACCR1 CCIFG, TACCR2 CCIFG, and TAIFG flags are prioritized and combined to source a single
 * interrupt vector. The interrupt vector register TAIV is used to determine which flag requested an interrupt.
 * The highest priority enabled interrupt generates a number in the TAIV register (see register description).
 * This number can be evaluated or added to the program counter to automatically enter the appropriate
 * software routine. Disabled Timer_A interrupts do not affect the TAIV value.
 * Any access, read or write, of the TAIV register automatically resets the highest pending interrupt flag. If
 * another interrupt flag is set, another interrupt is immediately generated after servicing the initial interrupt.
 * For example, if the TACCR1 and TACCR2 CCIFG flags are set when the interrupt service routine
 * accesses the TAIV register, TACCR1 CCIFG is reset automatically. After the RETI instruction of the
 * interrupt service routine is executed, the TACCR2 CCIFG flag will generate another interrupt.
 * */

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void) {
	switch (__even_in_range(TA0IV, TA0IV_TAIFG)) {
	case TA0IV_NONE: // No Interrupt pending
		break;
	case TA0IV_TACCR1: 	//TA0CCR1_CCIFG
		P1OUT &= ~LED1;
		P2OUT &= ~MOSFET;
		break;
	case TA0IV_TACCR2: 	//TA0CCR2_CCIFG
	case TA0IV_6:		//Reserved
	case TA0IV_8:		//Reserved
	case TA0IV_TAIFG:	//TA0IFG
	default:
		break;
	}
}
