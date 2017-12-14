/*
 * Params.cpp
 *
 *  Created on: 07.09.2016
 *      Author: lechu
 */

#include "eeprom.h"
#include "Parameter.h"
//#include "stm32f4xx.h"
#include "settings.h"

//Parameter::Parameter() {
//	// TODO Auto-generated constructor stub
//
//}

const Parameter::ParamConstDefs Parameter::paramDefsTable[LAST_PARAMETER+1] = {
		{ NONE, 0, 0, 0},
		{ MODE_AUTO_MANUAL, 	0,		1, 		1},
		{ TEMPERATURA_CO, 		20,		600,	980},
		{ HISTEREZA_CO_TEMP, 	1, 		20,		200},
		{ POMPA_CO_TEMP_ZALACZ, 200, 	450,	980},
		{ PODAJNIK_PRACA, 		1,		50,		300},
		{ PODAJNIK_PRZERWA,		10,		60*5,	60*60*10},
		{ PODAJNIK_OPOZNIENIE, 	1,		50,		300},
		{ PODTRZYMANIE_PRACA,	1, 		30,		200},
		{ PODTRZYMANIE_PRZERWA, 10,		60*6,	60*60*10},
		{ OPOZNIENIE_DMUCHAWY, 	1, 		20, 	300},
		{ DMUCHAWA_MOC, 		1, 		6, 		10},
		{ ALARM_SPADKU_TEMP, 	100,	450,	980},
		{ ALARM_TEMP_PODAJNIKA, 300, 	850,	1500},
		{ POMPA_CWU_TEMP_ZALACZ, 200,	450,	980},
		{ HISTEREZA_CWU_TEMP, 	1,		20,		200},
		{ TRYB_LETNI, 			0, 		0, 		1},
		{ AKTYWNA_CWU, 			0, 		0, 		1},
		{ USTAWIENIA_FABRYCZNE, 0, 		0,		1},	// znacznik powrotu do fabrycznych
		{ LAST_PARAMETER, 		0,		0,		1},
};


bool FlashUnlock(void);
bool FlashLock(void);

void clearFlashRegs();


//constexpr uint8_t NB_OF_VAR = 30;

uint16_t Parameter::nrOfParams = MAX_NR_OF_EEPROM_PARAMS;
uint16_t VirtAddVarTab[MAX_NR_OF_EEPROM_PARAMS];

uint16_t fastCopyofData[Parameter::Nazwa::LAST_PARAMETER + 1];

bool Parameter::fillFastDataTab(){
	bool result = true;
	for (uint16_t i = 1; i < Parameter::Nazwa::LAST_PARAMETER; i++){
		Nazwa nazwa = Nazwa(i);
		uint16_t val = 0;
		uint16_t stat = EE_ReadVariable(nazwa, &val);
		if (stat == 1){ // nie ma takiego jeszcze?
			val = getDefaultValue(nazwa);			// zaladowac wartosc domyslna
			stat = EE_WriteVariable(nazwa, val);
			if (stat != FLASH_COMPLETE){
				result =  false;
			}
		}else if (stat != 0){
			result =  false;
		}
		fastCopyofData[i] = val;
	}
	return result;
}

/**
 * @brief  Unlock the FLASH control register access
 * @retval HAL Status
 */
bool FlashUnlock(void){
	if((FLASH->CR & FLASH_CR_LOCK) != RESET)
	{
		/* Authorize the FLASH Registers access */
		FLASH->KEYR = FLASH_KEY1;
		FLASH->KEYR = FLASH_KEY2;
	} else{
		return false;
	}
	return true;
}

/**
 * @brief  Locks the FLASH control register access
 * @retval HAL Status
 */
bool FlashLock(void){
	/* Set the LOCK Bit to lock the FLASH Registers access */
	FLASH->CR |= FLASH_CR_LOCK;
	return true;
}


//void clearFlashRegs(){
//	if(__HAL_FLASH_GET_FLAG(FLASH_FLAG_WRPERR) != RESET)  {
//		/* Clear FLASH write protection error pending bit */
//		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_WRPERR);
//	}
//
//	if(__HAL_FLASH_GET_FLAG(FLASH_FLAG_PGAERR) != RESET)  {
//		/* Clear FLASH Programming alignment error pending bit */
//		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGAERR);
//	}
//
//	if(__HAL_FLASH_GET_FLAG(FLASH_FLAG_PGPERR) != RESET)  {
//		/* Clear FLASH Programming parallelism error pending bit */
//		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGPERR);
//	}
//
//	if(__HAL_FLASH_GET_FLAG(FLASH_FLAG_PGSERR) != RESET) {
//		/* Clear FLASH Programming sequence error pending bit */
//		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGSERR);
//	}
//#if defined(FLASH_SR_RDERR)
//	if(__HAL_FLASH_GET_FLAG(FLASH_FLAG_RDERR) != RESET)  {
//		/* Clear FLASH Proprietary readout protection error pending bit */
//		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_RDERR);
//	}
//#endif /* FLASH_SR_RDERR */
//	if(__HAL_FLASH_GET_FLAG(FLASH_FLAG_OPERR) != RESET)  {
//		/* Clear FLASH Operation error pending bit */
//		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPERR);
//	}
//}

bool Parameter::initEepromMemory(){
	bool result;
	FlashUnlock();
	//clearFlashRegs();
	result = (EE_Init() == FLASH_COMPLETE);
	//if (!result) result = (EE_Init() == FLASH_COMPLETE);
	fillFastDataTab();
	FlashLock();
	return result;
}



bool Parameter::setEEpromValue(uint16_t virtAdr, uint16_t value){
	if (virtAdr >= Nazwa::LAST_PARAMETER) return false;
	fastCopyofData[virtAdr] = value;
	FlashUnlock();
	uint16_t result = EE_WriteVariable(virtAdr,  value);
	FlashLock();
	return (result == FLASH_COMPLETE);
}

int32_t Parameter::getEEpromValue(uint16_t virtAdr){
	if (virtAdr >= Nazwa::LAST_PARAMETER) return -1;
	return fastCopyofData[virtAdr];

	//	uint16_t val = 0;
	//	uint16_t ret = EE_ReadVariable(virtAdr, &val);
	//	if (ret == 1){
	//		if (setEEpromValue(virtAdr, 0)) return 0;
	//		else return -1;
	//	}
	//	return (ret == FLASH_COMPLETE) ? val : -1;

}




