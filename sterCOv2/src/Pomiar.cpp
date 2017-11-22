/*
 * Pomiar.cpp
 *
 *  Created on: 7 wrz 2017
 *      Author: lechu
 */

#include "Pomiar.h"

//volatile uint16_t Pomiar::dataTable[count];


constexpr uint8_t  ADC_INPUTS = Pomiar::Analogi::count;

volatile uint16_t dataTable[ADC_INPUTS];

uint16_t Pomiar::getPomiarRaw(Pomiar::Analogi nr){
	if (nr >= ADC_INPUTS) return 0xfffe;
		return dataTable[nr];
}


uint16_t * Pomiar::getDataTablePtr(){ return (uint16_t*) dataTable; }
