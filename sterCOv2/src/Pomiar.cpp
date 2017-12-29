/*
 * Pomiar.cpp
 *
 *  Created on: 7 wrz 2017
 *      Author: lechu
 */

#include "Pomiar.h"

//volatile uint16_t Pomiar::dataTable[count];

const uint16_t * Pomiar::TS_CAL1_30oC = reinterpret_cast<uint16_t*>(0x01FFF7A2C);
const uint16_t * Pomiar::TS_CAL2_110oC = reinterpret_cast<uint16_t*>(0x1FFF7A2E);


const int32_t Pomiar::KTY81_2K_tempTab[KTY_SAMPLE_COUNT][KTY_COLUMNS] = {
		{0,		1630},
		{1000,	1772},
		{2000,	1922},
		{2500,	2000},
		{3000,	2080},
		{4000,	2245},
		{5000,	2417},
		{6000,	2597},
		{7000,	2785},
		{8000,	2980},
		{9000,	3182},
		{10000,	3392},
};


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

int32_t Pomiar::uPTemperature(uint16_t rawAdc){
	int32_t tadc = rawAdc;
	int32_t t30 = *TS_CAL1_30oC;
	int32_t t110 = *TS_CAL2_110oC;
	//int32_t dt = 11000-3000;
	return 30000 + ((11000-3000)*(tadc - t30))/(t110 - t30);
}


volatile uint16_t * Pomiar::getDataTablePtr(){ return  data.dataTable; }
