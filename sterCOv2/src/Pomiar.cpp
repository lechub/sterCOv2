/*
 * Pomiar.cpp
 *
 *  Created on: 7 wrz 2017
 *      Author: lechu
 */

#include "Pomiar.h"

//volatile uint16_t Pomiar::dataTable[count];


constexpr uint8_t  ADC_INPUTS = Pomiar::Analogi::count;

typedef union{
	volatile uint32_t dummy;
	volatile uint16_t dataTable[ADC_INPUTS * 2];
}DataPomiar;

DataPomiar data;


uint16_t Pomiar::getPomiarRaw(Pomiar::Analogi nr){
	if (nr >= ADC_INPUTS) return 0xfffe;
		return data.dataTable[nr];
}


volatile uint16_t * Pomiar::getDataTablePtr(){ return  data.dataTable; }
