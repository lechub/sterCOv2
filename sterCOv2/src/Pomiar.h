/*
 * Pomiar.h
 *
 *  Created on: 30.08.2016
 *      Author: lechu
 */

#ifndef INC_POMIAR_H_
#define INC_POMIAR_H_

//#include "stm32f4xx_hal.h"
//#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx.h"

//#include "system_stm32f4xx.h"



//	IN1	PC0	38		reg.pok.
//	IN2	PC3	37		czujnikPodaj.
//	IN3	PC1	36		czujnikCWU
//	IN4	PC2	35		CzujnikCo
//	IN5	PA4	32		termik



class Pomiar{

public:

	typedef enum {
		REG_POKOJ = 0,
		TEMP_PODAJNIKA = 1,
		TEMP_CO = 2,
		TEMP_CWU = 3,
		TERMIK = 4,
		UNUSED = 5,
		TEMP_uP = 6,
		count = 7,	// ilosc wejsc = 6
	} Analogi;

	static constexpr int32_t TEMPERARURE_ERROR = 999000;



private:
	//uint8_t nrAdc;
	static constexpr int32_t R1 = 1000;
	static constexpr int32_t U1 = 3300;
	static constexpr int32_t MAXADC = 4095;
	static constexpr int32_t RESISTANCE_ERROR = 10000;

	static uint16_t getPomiarRaw(Analogi nr);

	static constexpr uint8_t KTY_SAMPLE_COUNT = 12;
	static constexpr uint8_t KTY_COLUMNS = 2;
	static constexpr int32_t KTY81_2K_tempTab[KTY_SAMPLE_COUNT][KTY_COLUMNS] = {
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


	static int32_t aproximateCentigrades(int32_t resistance_mOhm){
		uint8_t zakres;
		for (zakres = 0; zakres < KTY_SAMPLE_COUNT; zakres++){
			if (resistance_mOhm < KTY81_2K_tempTab[zakres][1]) break;
		}
//
//		int32_t t1 = KTY81_2K_tempTab[zakres][0];
//		int32_t r1 = KTY81_2K_tempTab[zakres][1];
//		int32_t result = (resistance * t1)/r1;

		int32_t t1 = KTY81_2K_tempTab[zakres-1][0];
		int32_t r1 = KTY81_2K_tempTab[zakres-1][1];
		int32_t t2 = KTY81_2K_tempTab[zakres][0];
		int32_t r2 = KTY81_2K_tempTab[zakres][1];
		int32_t result = t1 + ((resistance_mOhm - r1)*(t2 - t1))/(r2 - r1);
		if ((result < -5000)||(result > 20000)) result = RESISTANCE_ERROR;
		return result;
	}

	static int32_t getResistance(uint16_t rawAdcValue){
		if (rawAdcValue >= MAXADC) return RESISTANCE_ERROR;
		return (R1 * rawAdcValue)/(MAXADC - rawAdcValue);
	}

	static int32_t rawAdcToCentigrade(uint16_t rawAdc){
		int32_t resist = getResistance(rawAdc);
		int32_t result = aproximateCentigrades(resist);
		return result;
	}


public:
	//	Pomiar(//EnumPomiar pom,
	//			uint8_t adcNr){
	//		nrAdc = adcNr;
	//	}

	static uint8_t getIloscWejsc(){ return count; };


	/** Zwraca pomiar w miliwoltach, miliamperach, miliCelsjuszach itd.
	 *
	 */
	static int32_t getPomiar(Analogi nrAdc){
		uint32_t result;
		result = getPomiarRaw(nrAdc);
		return result;
	}


	//	static void * getDataTablePtr(){ return (void*)dataTable; }
	static volatile uint16_t * getDataTablePtr();
};


#endif /* INC_POMIAR_H_ */
