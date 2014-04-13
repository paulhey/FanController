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

#ifndef ADC_SAMPLE_TYPE
#define ADC_SAMPLE_TYPE unsigned long
#endif

typedef struct{
		ADC_SAMPLE_TYPE current;
		unsigned char index;
		ADC_SAMPLE_TYPE samples[ADC_MAX_SAMPLES];
		ADC_SAMPLE_TYPE sum;
		ADC_SAMPLE_TYPE average;
		ADC_SAMPLE_TYPE calibrated;
}ADCSampleData;

void InitializeSamples(ADCSampleData *my_asd);
void UpdateSampleData(ADCSampleData *my_asd);

#endif /* SAMPLE_H_ */
