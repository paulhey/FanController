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
	if (my_asd->index == ADC_MAX_SAMPLES) my_asd->index = 0; //roll over index if needed
}
/*
void UpdateADCString(ADCSampleData *my_asd, char *my_string,
		unsigned int my_stringlength, unsigned char my_offset) {
	//TODO:if ((my_offset + 4) > my_stringlength) return;
	unsigned char thou = 0, hun = 0, ten = 0, one = 0;
	ADC_SAMPLE_TYPE temp = (my_asd->average);

	//RANGES: 10 bits is 0-1023 & 12 bits is 0-4096
	thou = temp / 1000;// [0-4] in [0-255]
	hun = (temp % 1000) / 100;//[0-9] in [0-255]
	ten = (temp % 100) / 10;//[0-9] in [0-255]
	one = (temp % 10);//[0-9] in [0-255]
	my_string[my_offset]=thou+ASCII_OFFSET;
	my_string[my_offset+1]=hun+ASCII_OFFSET;
	my_string[my_offset+2]=ten+ASCII_OFFSET;
	my_string[my_offset+3]=one+ASCII_OFFSET;
}
*/
