/*
 * sample.h
 *
 *  Created on: Apr 13, 2014
 *      Author: Paul
 */

#ifndef SAMPLE_H_
#define SAMPLE_H_

#define ADC_MAX_SAMPLES 8 //Use multiples of 2 for easy int division, 4 or 8 is best
#define ADC_SAMPLE_SHIFT 3 // 8 = 2^3, .: average = sum>>3, Use 2 or 3 respectively.
#define ASCII_OFFSET 0x30


#ifndef ADC_SAMPLE_TYPE
#define ADC_SAMPLE_TYPE unsigned int
#endif

typedef struct{
	unsigned long sum;
	ADC_SAMPLE_TYPE current;
	ADC_SAMPLE_TYPE samples[ADC_MAX_SAMPLES];
	ADC_SAMPLE_TYPE average;
	ADC_SAMPLE_TYPE calibrated;
	unsigned char index;
}ADCSampleData;

void InitializeSamples(ADCSampleData *my_asd);
void UpdateSampleData(ADCSampleData *my_asd);
void UpdateADCString(ADCSampleData *my_asd, char *my_string, unsigned int my_stringlength, unsigned char my_offset);

#endif /* SAMPLE_H_ */
