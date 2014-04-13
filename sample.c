/*
 * sample.c
 *
 *  Created on: Apr 13, 2014
 *      Author: Paul
 */
#include "sample.h"

//Set Calibrated before calling this function, will need to implement a second parameter.
void InitializeSamples(ADCSampleData *my_asd) {
	for (my_asd->index = 0; my_asd->index < ADC_MAX_SAMPLES; my_asd->index++) {
		my_asd->samples[my_asd->index] = my_asd->calibrated;
	}
	my_asd->average = my_asd->calibrated;
	my_asd->sum = my_asd->calibrated << ADC_SAMPLE_SHIFT;
	my_asd->index = 0;
}

void UpdateSampleData(ADCSampleData *my_asd) {
	my_asd->sum += my_asd->current; //Add newest value to sum
	my_asd->sum -= my_asd->samples[my_asd->index]; //subtract oldest value from sum
	my_asd->samples[my_asd->index] = my_asd->current; //overwrite oldest value with newest
	my_asd->average = my_asd->sum >> ADC_SAMPLE_SHIFT; //find the new average
	my_asd->index++; //Move index forward
	if (my_asd->index == ADC_MAX_SAMPLES)
		my_asd->index = 0; //roll over index if needed
}

