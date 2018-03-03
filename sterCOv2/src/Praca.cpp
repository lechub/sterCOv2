/*
 * Praca.c
 *
 *  Created on: 30.08.2016
 *      Author: lechu
 */

#include "Praca.h"
#include "Sterownik.h"
#include "QuickTask.h"

//Praca::Mode Praca::mode = START;
//uint32_t	Praca::modeTimeMs = 0;



Praca praca  = Praca();


/** Wywolanie metody monitor() */
void static inline praca_callback(){  praca.pracuj(); }


// periodycznie wykonywana funkcja monitor() opakowana w aku_callback()
QuickTask pracaQtsk(QuickTask::QT_PERIODIC, praca_callback, Praca::TIME_PERIOD_PRACUJ_MS);


void Praca::pracujManual(){
	checkAlarmy();
}

void Praca::pracujAutomat(){
	checkPompy();
	checkAlarmy();
	switch(mode){
	case START:	// tylko czeka na rozpalanie
		if (isModeTimeOut()) setMode(Praca::Mode::ROZPALANIE, MODE_ROZPALANIE_MS);
		break;
	case ROZPALANIE:	// dmuchanie i podawanie co jakis czas
		if (sterCO->getTempCO() >= Parameter::getValue(Parameter::Nazwa::TEMPERATURA_CO)){	// czy juz rozpalone?
			setMode(PODTRZYMANIE, 0);
		}
		if (isPodajnikTimeOut()){		//
			if (sterCO->podajnik->isTurnedOn()){	// koniec podawania
				sterCO->podajnik->turnOff();
				setPodajnikTime(Parameter::getValue(Parameter::Nazwa::PODAJNIK_PRZERWA));	// ustaw czas przerwy
			}else{
				sterCO->podajnik->turnOn();
				setPodajnikTime(Parameter::getValue(Parameter::Nazwa::PODAJNIK_PRACA));	// ustaw czas podawania
			}
		}
		break;
	case PODTRZYMANIE:{
		int32_t tempHi = sterCO->getTempCO();
		int32_t tempLo = tempHi - Parameter::getValue(Parameter::Nazwa::HISTEREZA_CO_TEMP);
		if ( tempLo > Parameter::getValue(Parameter::Nazwa::TEMPERATURA_CO)){
			break;
		}
		// ToDo sprawdzic jak to dziala dokladnie
		// temp ponizej histerezy
		if (isPodajnikTimeOut()){		//
			if (sterCO->podajnik->isTurnedOn()){	// koniec podawania
				sterCO->podajnik->turnOff();
				setPodajnikTime(Parameter::getValue(Parameter::Nazwa::PODTRZYMANIE_PRZERWA));	// ustaw czas przerwy
			}else{
				sterCO->podajnik->turnOn();
				setPodajnikTime(Parameter::getValue(Parameter::Nazwa::PODTRZYMANIE_PRACA));	// ustaw czas podawania
			}
		}
	}

	break;
	case WYGASZANIE:
		//if (isModeTimeOut()) setMode(ROZPALANIE, MODE_ROZPALANIE_MS);
		break;
	case MANUAL:
		if (isModeTimeOut()) setMode(START, MODE_START_MS);
		break;
	default:
		break;
	}
}



void Praca::setMode(Mode nowyTryb, uint32_t timeMs){
	mode = nowyTryb;
	modeTimeMs = timeMs;

	switch (nowyTryb){
	case START:
		sterCO->dmuchawa->turnOff();
		sterCO->pompaCO->turnOff();
		sterCO->pompaCWU->turnOff();
		sterCO->podajnik->turnOff();
		setDmuchawaTime(0);
		setPodajnikTime(0);
		break;
	case ROZPALANIE:
		sterCO->dmuchawa->turnOn();
		sterCO->pompaCO->turnOff();
		sterCO->pompaCWU->turnOff();
		sterCO->podajnik->turnOff();
		break;
	case PODTRZYMANIE:
		sterCO->dmuchawa->turnOff();
		sterCO->podajnik->turnOff();
		break;
	case WYGASZANIE:
		sterCO->dmuchawa->turnOff();
		sterCO->podajnik->turnOff();
		sterCO->pompaCO->turnOn();
		sterCO->pompaCWU->turnOn();
		break;
	case MANUAL:
		break;
	case STOP:
		sterCO->dmuchawa->turnOff();
		sterCO->pompaCO->turnOff();
		sterCO->pompaCWU->turnOff();
		sterCO->podajnik->turnOff();
		setDmuchawaTime(0);
		setPodajnikTime(0);
		break;

	default:
		sterCO->gotoSafePosition();
		break;

	}
}

/**
 * W zaleznosci od temperatury CO i CWU wlacza/wylacza pompy z ustawiona histereza.
 */
void Praca::checkPompy(){
	int32_t temp;
	// CO
	temp = sterCO->getTempCO();
	uint16_t par = Parameter::getValue(Parameter::Nazwa::POMPA_CO_TEMP_ZALACZ);
	if (temp >= par){
		sterCO->pompaCO->turnOn();
	}else{
		temp += Parameter::getValue(Parameter::Nazwa::HISTEREZA_CO_TEMP);
		if (temp < par){
			sterCO->pompaCO->turnOff();
		}
	}
	// CWU
	temp = sterCO->getTempCWU();
	par = Parameter::getValue(Parameter::Nazwa::POMPA_CWU_TEMP_ZALACZ);
	if (temp >= par){
		sterCO->pompaCWU->turnOn();
	}else{
		temp += Parameter::getValue(Parameter::Nazwa::HISTEREZA_CWU_TEMP);
		if (temp < par){
			sterCO->pompaCWU->turnOff();
		}
	}
}

void Praca::checkAlarmy(){
	int32_t tempCO = sterCO->getTempCO();
	int32_t tempCWU = sterCO->getTempCWU();
	int32_t tempPod = sterCO->getTempPodajnika();
//	uint32_t alarmPiec = Parameter::getValue(Parameter::Nazwa::ALARM_PIEC);
	uint32_t alarmSpadku = Parameter::getValue(Parameter::Nazwa::ALARM_SPADKU_TEMP);
	uint32_t alarmPodajnik = Parameter::getValue(Parameter::Nazwa::ALARM_TEMP_PODAJNIKA);
	bool alarm = false;
	if ( tempPod > alarmPodajnik ){
		setMode(WYGASZANIE,0);
		sterCO->setAlarmPodajnik();
		alarm = true;
	}

//	if ( tempCO < alarmPiecaCo ){
//		setMode(WYGASZANIE,0);
//		sterCO->setAlarmSpadekTemp();
//		alarm = true;
//	}

	if ( tempCO < alarmSpadku ){
		setMode(WYGASZANIE,0);
		sterCO->setAlarmSpadekTemp();
		alarm = true;
	}

}


