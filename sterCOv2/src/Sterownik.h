/*
 * Sterownik.h
 *
 *  Created on: 13.09.2016
 *      Author: lechu
 */

#ifndef STEROWNIK_H_
#define STEROWNIK_H_

#include "Led.h"
#include "Silnik.h"
#include "Pomiar.h"
#include "Praca.h"
#include "Parameter.h"
#include "Keyboard.h"
#include "Praca.h"


class Sterownik {

private:
//	Gpio * gpioPompaCO; // = Gpio(GPIOA,1);
//	Gpio * gpioPompaCWU; // = Gpio(GPIOA,1);
//	Gpio * gpioDmuchawa; // = Gpio(GPIOA,1);
//	Gpio * gpioPodajnik; // = Gpio(GPIOA,1);

	bool alarmPodajnik = false;
	bool alarmSpadkuTemp = false;
	Praca praca = Praca();

public:

	Silnik * pompaCO; //= Silnik(gpioPompaCO);
	Silnik * pompaCWU; //= Silnik(gpioPompaCWU);
	Silnik * dmuchawa; //= Silnik(gpioDmuchawa);
	Silnik * podajnik; // = Silnik(gpioPodajnik);



	Sterownik(Silnik * sPompaCO, Silnik * gPompaCWU, Silnik * gDmuchawa, Silnik * gPodajnik
			//, Keyboard * keyb, TextLcd * lcdTxt
			){
		pompaCO = sPompaCO;
		pompaCWU = gPompaCWU;
		dmuchawa = gDmuchawa;
		podajnik = gPodajnik;
		alarmPodajnik = false;
		alarmSpadkuTemp = false;
		//keys = keyb;
		//lcd = lcdTxt;


	}


	// sterowania
	//	Led ledAlarm	= Led(GPIOA,1, Led::Type::ACTIVELOW, Led::Mode::SWIECI );
	//	Led ledWentylator	= Led(GPIOA,1, Led::Type::ACTIVELOW, Led::Mode::SWIECI);
	//	Led ledPodajnik	= Led(GPIOA,1, Led::Type::ACTIVELOW, Led::Mode::SWIECI);
	//	Led ledPompaCO	= Led(GPIOA,1, Led::Type::ACTIVELOW, Led::Mode::SWIECI);
	//	Led ledPompaCWU	= Led(GPIOA,1, Led::Type::ACTIVELOW, Led::Mode::SWIECI);


	// czujniki i przyciski

	//	Gpio btnMenu = Gpio(GPIOA,1);
	//	Gpio btnOK = Gpio(GPIOA,1);
	//	Gpio btnUp = Gpio(GPIOA,1);
	//	Gpio btnDown = Gpio(GPIOA,1);
	//
	//	Gpio regulatorPokojowy = Gpio(GPIOA,1);
	//	Gpio termik = Gpio(GPIOA,1);


	int32_t getTempPodajnika(){return Pomiar::getPomiar(Pomiar::Analogi::TEMP_PODAJNIKA)/10; }
	int32_t getTempCO(){return Pomiar::getPomiar(Pomiar::Analogi::TEMP_CO)/10; }
	int32_t getTempCWU(){return Pomiar::getPomiar(Pomiar::Analogi::TEMP_CWU)/10; }

//	Pomiar tempPodajnika = Pomiar(1);
//	Pomiar tempCO = Pomiar(2);
//	Pomiar tempCWU = Pomiar(3);


//	Parameter parTempUstawione	= Parameter((char*)"TEMPERAT. USTAW.");
//	Parameter parPodajnikPraca	= Parameter((char*)"PODAJNIK_PRACA");
//	Parameter parPodajnikPrzerwa	= Parameter((char*)"PODAJNIK_PRZERWA");
//	Parameter parPodajnikOpozn	= Parameter((char*)"PODAJNIK_OPOZNIENIE");
//	Parameter parPodtrzPraca	= Parameter((char*)"PODTRZYMANIE_PRACA");
//	Parameter parPodtrzPrzerwa	= Parameter((char*)"PODTRZYMANIE_PRZERWA");
//	Parameter parOpoznDmuchawy	= Parameter((char*)"OPOZNIENIE_DMUCHAWY");
//	Parameter parTempZalaczPompyCO	= Parameter((char*)"TEMP. ZALACZ. CO");
//	Parameter parAlarmSpadkuTemp	= Parameter((char*)"ALARM_SPADKU_TEMP");
//	Parameter parMocDmuchawy	= Parameter((char*)"DMUCHAWA_MOC");
//	Parameter parHisterezaCO	= Parameter((char*)"HISTEREZA_CO");
//	Parameter parTempZalaczPompyCWU	= Parameter((char*)"POMPA_CWU_TEMP_ZALACZ");
//	Parameter parHisterezaCWU	= Parameter((char*)"HISTEREZA_CWU_TEMP");
//	Parameter parTrybLetni		= Parameter((char*)"TRYB_LETNI");
//	Parameter parAktywnaCWU		= Parameter((char*)"ALARM_TEMP_PODAJNIKA");
//	Parameter parAlarmTempPodajnika	= Parameter((char*)"ALARM_TEMP_PODAJNIKA");


	//Keyboard *keys;

	//TextLcd * lcd;



	Praca * getPraca(){ return &praca;}

	bool gotoSafePosition(){
		dmuchawa->turnOff();
		podajnik->turnOff();
		pompaCO->turnOn();
		pompaCWU->turnOn();
		return false;
	}

	bool setAlarmPodajnik(){
		alarmPodajnik = true;
		gotoSafePosition();
		return true;
	}

	bool setAlarmSpadekTemp(){
		alarmSpadkuTemp = true;
		gotoSafePosition();
		return true;
	}

};

extern Sterownik * sterCO;

#endif /* STEROWNIK_H_ */
