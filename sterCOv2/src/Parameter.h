/*
 * Params.h
 *
 *  Created on: 07.09.2016
 *      Author: lechu
 */

#ifndef INC_PARAMETER_H_
#define INC_PARAMETER_H_

#include "stdint.h"
#include "eeprom.h"

//extern constexpr uint8_t NB_OF_VAR = 30;


class Parameter {
private:
	static uint16_t nrOfParams;// = 0;

	uint16_t nr; 		// indeks parametru w eepromie
	uint16_t actualValue;
	const char * name;

	static bool setEEpromValue(uint16_t virtAdr, uint16_t value);
	static int32_t getEEpromValue(uint16_t virtAdr);
	static bool fillFastDataTab();

public:

	typedef enum{
		NONE = 0,
		MODE_AUTO_MANUAL = 1,
		TEMPERATURA_CO,
		HISTEREZA_CO_TEMP,
		POMPA_CO_TEMP_ZALACZ,
		PODAJNIK_PRACA,
		PODAJNIK_PRZERWA,
		PODAJNIK_OPOZNIENIE,
		PODTRZYMANIE_PRACA,
		PODTRZYMANIE_PRZERWA,
		OPOZNIENIE_DMUCHAWY,
		DMUCHAWA_MOC,
		ALARM_SPADKU_TEMP,
		ALARM_TEMP_PODAJNIKA,
		POMPA_CWU_TEMP_ZALACZ,
		HISTEREZA_CWU_TEMP,
		TRYB_LETNI,
		AKTYWNA_CWU,
		USTAWIENIA_FABRYCZNE,		// znacznik powrotu do fabrycznych
		LAST_PARAMETER
	}Nazwa;

private:
	typedef struct{
		Nazwa		param;
		uint16_t	minValue;
		uint16_t	defValue;
		uint16_t	maxValue;
	}ParamConstDefs;

	static const ParamConstDefs paramDefsTable[LAST_PARAMETER+1];

	static const ParamConstDefs * getConstDefs(Nazwa nazwa){
		for (uint8_t i = 0; i< LAST_PARAMETER; i++){
			if (paramDefsTable[i].param == nazwa) return &paramDefsTable[i];
		}
		return nullptr;
	}

public:
	typedef enum{
		LATO = 1,
		ZIMA = 2,
	}SEZON;

	//	Parameter(const char * opis){
	//		nr = nrOfParams;
	//		nrOfParams++;
	//		int32_t tmp = getEEpromValue(nr);
	//		if (tmp < 0) actualValue = 0;
	//		name = opis;
	//	}

	static int32_t getValue(Nazwa nazwa){
		return getEEpromValue(uint16_t(nazwa));
	}

	static bool setValue(Nazwa nazwa, uint16_t newValue){
		newValue = adjustValueToFit(nazwa, newValue);
		return setEEpromValue(nazwa, newValue);
	}

	static const char * getParamPattern(Nazwa nazwa){
		switch(nazwa){
		case Parameter::Nazwa::PODAJNIK_PRACA:
		case Parameter::Nazwa::PODAJNIK_OPOZNIENIE:
		case Parameter::Nazwa::PODTRZYMANIE_PRACA:
		case Parameter::Nazwa::OPOZNIENIE_DMUCHAWY:
			//------>1234567890123456<
			return	"   00.0 sek.    ";

		case Parameter::Nazwa::TEMPERATURA_CO:
			//------>1234567890123456<
			return	"T. ustaw:00.0 oC";

		case Parameter::Nazwa::PODAJNIK_PRZERWA:
		case Parameter::Nazwa::PODTRZYMANIE_PRZERWA:
			//------>1234567890123456<
			return  " 00 min. 60 sek.";

		case Parameter::Nazwa::HISTEREZA_CO_TEMP:
		case Parameter::Nazwa::HISTEREZA_CWU_TEMP:
			//------>1234567890123456<
			return	"temp: 00.0 st.C ";

		case Parameter::Nazwa::POMPA_CO_TEMP_ZALACZ:
		case Parameter::Nazwa::POMPA_CWU_TEMP_ZALACZ:
		case Parameter::Nazwa::ALARM_TEMP_PODAJNIKA:
			//------>1234567890123456<
			return  "wlacz >00.0 oC  ";

		case Parameter::Nazwa::DMUCHAWA_MOC:
			//------>1234567890123456<
			return  "     = 00 =     ";

		case Parameter::Nazwa::ALARM_SPADKU_TEMP:
			//------>1234567890123456<
			return  "mniej od 00.0 oC";

		case Parameter::Nazwa::TRYB_LETNI:
		case Parameter::Nazwa::AKTYWNA_CWU:
		case Parameter::Nazwa::USTAWIENIA_FABRYCZNE:
		case Parameter::Nazwa::MODE_AUTO_MANUAL:
		case Parameter::Nazwa::NONE:
		case Parameter::Nazwa::LAST_PARAMETER:
		default:
			return "??";
		}
	}

	static bool initEepromMemory();


	static uint16_t getDefaultValue(Nazwa nazwa){
		const ParamConstDefs * par = getConstDefs(nazwa);
		if (par == nullptr) return 10;
		return par->defValue;
	}



	static uint16_t adjustValueToFit(Nazwa nazwa, uint32_t value){
		const ParamConstDefs * par = getConstDefs(nazwa);
		if (par == nullptr) return 99;
		if (value > par->maxValue) value = par->maxValue;
		if (value < par->minValue) value = par->minValue;
		return uint16_t(value);
	}


	static bool saveDefaultValues(){
		bool result = true;
		for (uint16_t i = 1; i < uint16_t(Nazwa::LAST_PARAMETER); i++){
			Nazwa param = Nazwa(i);
			if (!setValue(param, getDefaultValue(param) )) result = false;
		}
		return result;
	}


};



//	static uint16_t getDefaultValue(Nazwa nazwa){
//		switch(nazwa){
//		case Nazwa::MODE_AUTO_MANUAL:		return uint16_t(true);
//		case Nazwa::TEMPERATURA_CO: 		return 600u;
//		case Nazwa::HISTEREZA_CO_TEMP:		return 20u;
//		case Nazwa::POMPA_CO_TEMP_ZALACZ:	return 450u;
//		case Nazwa::PODAJNIK_PRACA: 		return 50u;
//		case Nazwa::PODAJNIK_PRZERWA:		return 60*5;
//		case Nazwa::PODAJNIK_OPOZNIENIE: 	return 50u;
//		case Nazwa::PODTRZYMANIE_PRACA: 	return 20u;
//		case Nazwa::PODTRZYMANIE_PRZERWA:	return 60*6;
//		case Nazwa::OPOZNIENIE_DMUCHAWY:	return 20u;
//		case Nazwa::DMUCHAWA_MOC: 			return 6u;
//		case Nazwa::ALARM_SPADKU_TEMP:		return 450u;
//		case Nazwa::ALARM_TEMP_PODAJNIKA:	return 850u;
//		case Nazwa::POMPA_CWU_TEMP_ZALACZ:	return 450u;
//		case Nazwa::HISTEREZA_CWU_TEMP:		return 20u;
//		case Nazwa::TRYB_LETNI:				return uint16_t(false);
//		case Nazwa::AKTYWNA_CWU:			return uint16_t(false);
//		case Nazwa::NONE:
//		case Nazwa::USTAWIENIA_FABRYCZNE:
//		case Nazwa::LAST_PARAMETER:
//		default:
//			break;
//		}
//		return 100u;
//	}



//	static bool setDefaultValues(){
//		for (uint16_t i = 1; i < uint16_t(Nazwa::LAST_PARAMETER); i++){
//			Nazwa param = Nazwa(i);
//			switch(param){
//			case Nazwa::MODE_AUTO_MANUAL:		setValue(param, uint16_t(true) ); break;
//			case Nazwa::TEMPERATURA_CO: 		setValue(param, 600u ); break;
//			case Nazwa::HISTEREZA_CO_TEMP:		setValue(param, 20u ); break;
//			case Nazwa::POMPA_CO_TEMP_ZALACZ:	setValue(param, 450u ); break;
//			case Nazwa::PODAJNIK_PRACA: 		setValue(param, 50u ); break;
//			case Nazwa::PODAJNIK_PRZERWA:		setValue(param, 60*5 ); break;
//			case Nazwa::PODAJNIK_OPOZNIENIE: 	setValue(param, 50u ); break;
//			case Nazwa::PODTRZYMANIE_PRACA: 	setValue(param, 20u ); break;
//			case Nazwa::PODTRZYMANIE_PRZERWA:	setValue(param, 60*6 ); break;
//			case Nazwa::OPOZNIENIE_DMUCHAWY:	setValue(param, 20u ); break;
//			case Nazwa::DMUCHAWA_MOC: 			setValue(param, 6u ); break;
//			case Nazwa::ALARM_SPADKU_TEMP:		setValue(param, 450u ); break;
//			case Nazwa::ALARM_TEMP_PODAJNIKA:	setValue(param, 850u ); break;
//			case Nazwa::POMPA_CWU_TEMP_ZALACZ:	setValue(param, 450u ); break;
//			case Nazwa::HISTEREZA_CWU_TEMP:		setValue(param, 20u ); break;
//			case Nazwa::TRYB_LETNI:				setValue(param, uint16_t(false) ); break;
//			case Nazwa::AKTYWNA_CWU:			setValue(param, uint16_t(false) ); break;
//
//			case Nazwa::NONE:
//			case Nazwa::USTAWIENIA_FABRYCZNE:
//			case Nazwa::LAST_PARAMETER:
//			default:
//				break;
//			}
//		}
//	}




//	typedef struct {
//		ParamEnum paramEnum;
//		char * 	paramName;
//	}ParamEntity;

//	static const ParamEntity paramEntities [] = {
//	{PODAJNIK_PRACA, "PODAJNIK_PRACA"},
//	{PODAJNIK_PRZERWA, "PODAJNIK_PRZERWA"},
//	{PODAJNIK_OPOZNIENIE, "PDAJNIK_OPOZNIENIE"},
//	{PODTRZYMANIE_PRACA, "PODTRZYMANIE_PRACA"},
//	{PODTRZYMANIE_PRZERWA, "PODTRZYMANIE_PRZERWA"},
//	{OPOZNIENIE_DMUCHAWY, "OPOZNIENIE_DMUCHAWY"}
//	};



//
//	static constexpr uint32_t maxNrOfParams = LAST_PARAMETER;


//1.Praca ręczna
//2.Podajnik opału
// 2.1 Podajnik praca(min, 4 sek)
// 2.2 Podajnik przerwa(5 min, sek)
// 2.3 Podajnik opoznienie(min, 30 sek)
//3. Podtrzymanie palenia
// 3.1 Podtrzymanie praca ( 3 sek) 1-59
// 3.2 Podtrzymanie przerwa ( 29 min )
// 3.3 Opoznienie wyłączenia dmuchawy (25 sek)
//4. Pompa C.O.
// Temp zal. 40 oC 50-80
//5. Alarm spadku temperatury // 1 godz min
//6. Dmuchawa// moc 1-4
//7. Histereza// 2 oC (2-9)
//8. Pompa CWU// Temp załączenia 50 oC
// 8.1 Histereza 3 oC
// 8.2 Tryb letni nieaktywny/aktywny
//9. Opcje serwisowe
// 9.1 Ustawienia fabryczne
// 9.2 Pompa CWU aktywna/nieaktywna
// 9.3 Temperatura podajnika alarm 85 oC




#endif /* INC_PARAMETER_H_ */
