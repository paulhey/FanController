#include <msp430.h> 
#include "main.h"
#include "sample.h"
//#include <stdio.h>
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
const char OutputString[] = { "\r\nHello World" };
const char EchoString[] =
		{
				"\r\nFan Controller Module:\r\n\
		t => Current Temperature\r\n\
		u => Hello\r\n\
		e => Echo this prompt\r\n\
		a => Automatic Mode (Default, 25 \xB0 C)\r\n\
		s => Setup Mode \r\n\
		m => Manual Mode \r\n\
		l => Under Setup Mode, Low Threshold input\r\n" };
//						     0          1            2            3         4		  5
//-------------------Char: "0 1 234567890123   456789012345   6789012345678901234567890123456789"
char TemperatureString[] = "\r\n[TTTT->tttt\xB0 C] TL[tttt\xB0 C] PWM[n] or RAW[llll] Mode[m]";
const char SetupString[] = "\r\nSetup Mode";
const char AutoString[] = "\r\nAuto Mode";
const char DebugString[] = "\r\nDebug Mode";
const char ManualString[] = "\r\nManual Mode";
const char LowString[] =
		"\r\nEnter Low Threshold in \xB0 C (30% duty cycle below this point, default is 25\xB0 ):"; //ten, one, enter=="\r"?
const char ErrorString[] = "\r\nERROR: Input must be between 00-99";

struct {
	unsigned char RxBuffer;
	unsigned char RxTemp;
	unsigned int TxTemp;
	unsigned int ms_Counter;	//Keep track of current ms count
	unsigned char TS_i;
	unsigned int Tx_i;
	const char *p_TxString;
	unsigned int TxStringLength;
	ADCSampleData Temperature;
	int degreesC;
	int lowThresholdC;
	int diffC;
	int DC_slope;
	unsigned int *p_ADC10CalData;
	//Temperature = (RawADC-CAL_ADC_15T30) x [ (85-30)/(CAL_ADC_15T85-CAL_ADC_15T30)] + 30
	// (=) 		  = [((RawADC-Low) x 50)/(High-Low)]+30
	unsigned int Low;
	unsigned int High;
	unsigned char pwmDutyLevel;
	SystemMode sm;
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
	SetupTIMER0_A();
	SetupTIMER1_A();
	__enable_interrupt();
	__bis_SR_register(LPM0_bits + GIE);

	//return 0;
}

void init(void) {
	GV.RxBuffer = 0x00;
	GV.RxTemp = 0;
	GV.ms_Counter = 0x0000;
	GV.TS_i = 0;
	GV.Tx_i = 0;
	GV.p_TxString = EchoString;
	GV.TxStringLength = sizeof(EchoString);
	GV.degreesC = 0;
	GV.lowThresholdC = 25;
	GV.diffC = GV.degreesC - GV.lowThresholdC;
	GV.DC_slope = 2;
	GV.p_ADC10CalData = (unsigned int *) &TLV_ADC10_1_TAG;
	GV.High = *(GV.p_ADC10CalData + CAL_ADC_15T85 + 1);
	GV.Low = *(GV.p_ADC10CalData + CAL_ADC_15T30 + 1);
	GV.pwmDutyLevel = 5;
	GV.sm = _MANUAL_MODE;

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

void SetupTIMER0_A(void) {
	TA0CCR0 = TIMER0_A0_MAX;
	TA0CCR1 = TIMER0_A1_STEP * GV.pwmDutyLevel;  	//For ~50% Duty Cycle
	TA0CCTL0 |= CCIE;
	TA0CCTL1 |= CCIE;
	TA0CTL = TASSEL_2 + MC_1 + TACLR;  	//SMCLK, UP, CLEAR
}

void SetupTIMER1_A(void) {
	TA1CCR0 = TIMER1_A0_MAX;
	TA1CCTL0 |= CCIE;
	TA1CTL = TASSEL_2 + MC_1 + TACLR;
}

//Setup the ADC10 for temperature conversion.
//V TEMP =0.00355(TEMP C )+0.986
//.: TEMP C = (V TEMP - 0.986)/0.00355
//[?](=) TEMP C = (V TEMP - 0.986) * 281.69[/?]
//Since ADC range is 10 bits (0x0000 to 0x03FF) (0.0V to 3V?)
//(=) TEMP C = (V TEMP
void SetupADC10(void) {
	//Temp Sensor, ADC10CLK/4
	ADC10CTL1 = INCH_10 + ADC10DIV_3;
	//(VREF & VSS), 64xADC10CLKS, Use Reference, Turn on, use interrupt
	ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON + ADC10IE;
	__delay_cycles(ADC10_REF_SETTLE_TIME);
	ADC10CTL0 |= ENC + ADC10SC;
	GV.Temperature.calibrated = ADC10MEM;  	//Initialize samples array
	InitializeSamples(&GV.Temperature);
	GV.TxTemp = GV.Temperature.calibrated;
	ConvertRawToTemp(GV.Temperature.average, &(GV.degreesC));
}

//Start Transmitting the GV TxString
void TransmitGVTxString(const char temp[], unsigned char length) {
	GV.p_TxString = temp;
	GV.TxStringLength = length;
	GV.Tx_i = 0;
	IE2 |= UCA0TXIE;	// Enable USCI_A0 TX interrupt
	UCA0TXBUF = GV.p_TxString[GV.Tx_i++];
}

void ConvertRawToTemp(unsigned int my_raw, int *my_temp) {
	int t1, t2, t3, t4, t5;
	t1 = my_raw - GV.Low;
	t2 = t1 * 50;
	t3 = GV.High - GV.Low;
	t4 = t2 / t3;
	t5 = t4 + 30;
	*my_temp = t5;
//my_temp = (((my_raw-GV.Low) * 50)/(GV.High-GV.Low))+30;
//my_temp = (unsigned int) ((my_raw - l) * (85 - 35));	//TODO: Here
//my_temp /= (h-l);
//my_temp += 30;
}

//ISRs=================================================================

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0_TX_ISR(void) {
	UCA0TXBUF = GV.p_TxString[GV.Tx_i++]; // TX next character
	if (GV.Tx_i == GV.TxStringLength - 1) { // TX over?
		IE2 &= ~UCA0TXIE; // Disable USCI_A0 TX interrupt
	}
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0_RX_ISR(void) {
//while(!(IFG2&UCA0TXIFG));
//UCA0TXBUF = UCA0RXBUF;
	GV.RxBuffer = UCA0RXBUF;
	if (GV.sm != _SETUP_ENTER) {
		switch (GV.RxBuffer) {
			case 'u': //Hello World
				TransmitGVTxString(OutputString, sizeof(OutputString));
				break;
			case 'e': //Echo String
				TransmitGVTxString(EchoString, sizeof(EchoString));
				break;
			case 't': //Display current raw temp, degrees c, pwm level
				TransmitGVTxString(TemperatureString,
						sizeof(TemperatureString));
				break;
			case 'l': //SETUP_MODE: Display message and prompt
				if (GV.sm == _SETUP_MODE) {
					TransmitGVTxString(LowString, sizeof(LowString)); //TODO
					GV.sm = _SETUP_TEN;
				}
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '0':
				UCA0TXBUF = GV.RxBuffer;
				if (GV.sm == _MANUAL_MODE) { //Set as multiple of TIMER0_A1_STEP, duty cycle is 100% x (TIMER0_A1_STEP + 1)/(TIMER0_A0_MAX)
					GV.pwmDutyLevel = (GV.RxBuffer - ASCII_OFFSET ? : 10); //[https://en.wikipedia.org/wiki/%3F:#C]
					break;
				}
				if (GV.sm == _SETUP_TEN) { //Degrees * 10
					GV.RxTemp = 10 * (GV.RxBuffer - ASCII_OFFSET);
					GV.sm = _SETUP_ONE;
					break;
				}
				if (GV.sm == _SETUP_ONE) { //Degrees * 1
					GV.RxTemp += (GV.RxBuffer - ASCII_OFFSET);

					GV.sm = _SETUP_ENTER;
					break;
				}
				break;
			case 'a':
				if (GV.sm != _AUTO_MODE) {
					TransmitGVTxString(AutoString, sizeof(AutoString));
					GV.sm = _AUTO_MODE;
				}
				UCA0TXBUF = GV.RxBuffer;
				break;
			case 'd':
				if (GV.sm != _DEBUG_MODE) {
					TransmitGVTxString(DebugString, sizeof(DebugString));
					GV.sm = _DEBUG_MODE;
				}
				UCA0TXBUF = GV.RxBuffer;
				break;
			case 's':
				if (GV.sm != _SETUP_MODE) {
					TransmitGVTxString(SetupString, sizeof(SetupString));
					GV.sm = _SETUP_MODE;
				} else {
					TransmitGVTxString(AutoString, sizeof(AutoString));
					GV.sm = _AUTO_MODE;
				}
				UCA0TXBUF = GV.RxBuffer;
				break;
			case 'm':
				GV.sm = _MANUAL_MODE;
				TransmitGVTxString(ManualString, sizeof(ManualString));
				UCA0TXBUF = GV.RxBuffer;
				break;
			case '\e':
				if ((GV.sm == _SETUP_TEN) || (GV.sm == _SETUP_ONE)) {
					TransmitGVTxString(SetupString, sizeof(SetupString));
					GV.sm =	_SETUP_MODE;
				}
				break;
			default:
				UCA0TXBUF = '\r';
				break;
		}
	} else {
		if (GV.RxBuffer == '\r') {
			GV.lowThresholdC = (int) (GV.RxTemp);
			GV.RxTemp = 0;
			TransmitGVTxString(AutoString, sizeof(AutoString));
			GV.sm = _AUTO_MODE;
		}
	}
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void) {
	P1OUT |= LED1;
	P2OUT |= MOSFET;
	if (GV.sm == _MANUAL_MODE) {
		TA0CCR1 = (TIMER0_A1_STEP * GV.pwmDutyLevel) + 1;	//Updates DUTY Cycle
	} else {
		if (GV.diffC > 0) {
			TA0CCR1 = (TIMER0_A1_STEP + 1 + (unsigned int) GV.DC_slope);//TODO
		} else {
			TA0CCR1 = (TIMER0_A1_STEP + 1);
		}
	}
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
__interrupt
void TIMER0_A1_ISR(void) {
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

#pragma vector=ADC10_VECTOR
__interrupt
void ADC10_ISR(void) {
	GV.Temperature.current = ADC10MEM;
	UpdateSampleData(&GV.Temperature);
	GV.TxTemp = GV.Temperature.average;

	UpdateADCString(GV.TxTemp, TemperatureString, sizeof(TemperatureString),
	TX_TEMP_OFFSET);

	UpdateADCString(GV.degreesC, TemperatureString, sizeof(TemperatureString),
	DEGREESC_OFFSET);

	TemperatureString[PWM_DUTY_LEVEL_OFFSET] = (
			GV.pwmDutyLevel == 10 ? '0' : GV.pwmDutyLevel + ASCII_OFFSET);
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt
void TIMER1_A0_ISR(void) {
	GV.ms_Counter++;
	if (GV.ms_Counter == MS_COUNTER_MAX) {	//Toggle every second

		P1OUT ^= LED2;
		GV.ms_Counter = 0;	//Reset Counter
		if (GV.sm == _DEBUG_MODE) {
			TransmitGVTxString(TemperatureString, sizeof(TemperatureString));
		}
	}
	ConvertRawToTemp(GV.Temperature.average, &(GV.degreesC));
	GV.diffC = GV.degreesC - GV.lowThresholdC;
	GV.DC_slope = SLOPE_RATE * GV.diffC;	//((180 * GV.diffC) / 75);
//Update the rest of the Temp String:
//"\r\n[TTTT->tttt\xB0 C] TL[tttt\xB0 C] PWM[n] or RAW[llll] Mode[m]"
	UpdateADCString((TIMER0_A1_STEP + 1 + (unsigned int) GV.DC_slope), TemperatureString, sizeof(TemperatureString),
		PWM_RAW_VALUE_OFFSET);
	UpdateADCString(GV.lowThresholdC, TemperatureString, sizeof(TemperatureString),
			THRESHOLD_OFFSET);
	TemperatureString[MODE_OFFSET]=GV.sm;
//Trigger next ADC? ADC10CTL0 |= ENC + ADC10SC;
	if (!(ADC10CTL0 & ADC10SC)) {
		ADC10CTL0 |= ENC + ADC10SC;
	}
//Handle System State Machine
	/*
	 switch (GV.sm) {
	 case _AUTO_MODE:

	 break;
	 case _MANUAL_MODE:
	 //break;
	 case _SETUP_MODE:
	 //break;
	 default:
	 break;
	 }*/

}

/*
 #pragma vector=TIMER1_A1_VECTOR
 __interrupt void TIMER1_A1_ISR(void) {
 switch (__even_in_range(TA1IV, TA1IV_TAIFG)) {
 case TA1IV_NONE: // No Interrupt pending
 break;
 case TA1IV_TACCR1: 	//TA1CCR1_CCIFG
 P1OUT &= ~LED2;
 break;
 case TA1IV_TACCR2: 	//TA1CCR2_CCIFG
 case TA1IV_6:		//Reserved
 case TA1IV_8:		//Reserved
 case TA1IV_TAIFG:	//TA1IFG
 default:
 break;
 }
 }
 */
