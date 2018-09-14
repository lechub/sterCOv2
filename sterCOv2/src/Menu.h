/*
 * Menu.h
 *
 *  Created on: 04.07.2017
 *      Author: lechu
 */

#ifndef MENU_H_
#define MENU_H_

#include "Keyboard.h"
#include "FrameBuffer.h"
#include "Sterownik.h"
//#include ""
//#include "HMI.h"
#include "Parameter.h"

class Menu {

public:
	typedef enum{
		e_AUTOMAT,
		e1_PRACA_RECZNA,
		e2_PODAJNIK_OPALU,
		e2_1_PODAJNIK_PRACA,
		e2_2_PODAJNIK_PRZERWA,	//(5 min, sek),
		e2_3_PODAJNIK_OPOZNIENIE, //(min, 30 sek),
		e3_PODTRZYMANIE_PALENIA,
		e3_1_PODTRZYMANIE_PRACA, // ( 3 sek) 1-59,
		e3_2_PODTRZYMANIE_PRZERWA, //( 29 min ),
		e3_3_OPOZNIENIE_WLACZENIA_DMUCHAWY, //(25 sek),
		e4_POMPA_CO,		//Temp zal. 40 oC 50-80,
		e5_ALARM_SPADKU_TEMPERATURY, //, godz min,
		e6_Dmuchawa,			//moc 1-4,
		e7_HISTEREZA_CO,		//2 oC (2-9),
		e8_POMPA_CWU,	//Temp załączenia 50 oC,
		e8_1_HISTEREZA_CWU, //3 oC,
		e8_2_TRYB_LETNI, //nieaktywny/aktywny,
		e9_OPCJE_SERWISOWE,
		e9_1_USTAWIENIA_FABRYCZNE,
		e9_2_POMPA_CWU_AKTYWNA, // aktywna/nieaktywna,
		e9_3_ALARM_TEMP_PODAJNIKA,// alarm 85 oC,
		e_DEBUG_INFO,		// ekran z informacjami debugowymi
	}EKRAN;



	//HMI * hmi;

private:

	static constexpr uint32_t REFRESH_DELAY_MS = 1888;

	Sterownik * ster;
	Keyboard * keys;
	FrameBuffer * lcd;
	Praca * praca;

	uint32_t refreshDelay;

	EKRAN ekran;
	Parameter::Nazwa editParam = Parameter::Nazwa::NONE;
	uint16_t editValue = 0;
	bool editMode = false;

public:

	void init(Sterownik * pSter, Keyboard * pKeys, FrameBuffer * pLcd){
		ster = pSter;
		keys = pKeys;
		lcd = pLcd;
		praca = ster->getPraca();

		ekran = EKRAN::e_AUTOMAT;
		//goToEkran(EKRAN::e_AUTOMAT);
	}


	void goToEkran(EKRAN nowyEkran){
		ekran = nowyEkran;
		editMode = false;

		switch(ekran){
		case e_AUTOMAT:					editParam = Parameter::Nazwa::TEMPERATURA_CO; break;
		case e1_PRACA_RECZNA:			editParam = Parameter::Nazwa::NONE; break;
		case e2_PODAJNIK_OPALU:			editParam = Parameter::Nazwa::NONE; break;
		case e2_1_PODAJNIK_PRACA:		editParam = Parameter::Nazwa::PODAJNIK_PRACA; break;
		case e2_2_PODAJNIK_PRZERWA:		editParam = Parameter::Nazwa::PODAJNIK_PRZERWA; break;
		case e2_3_PODAJNIK_OPOZNIENIE:	editParam = Parameter::Nazwa::PODAJNIK_OPOZNIENIE; break;
		case e3_PODTRZYMANIE_PALENIA:	editParam = Parameter::Nazwa::NONE; break;
		case e3_1_PODTRZYMANIE_PRACA:	editParam = Parameter::Nazwa::PODTRZYMANIE_PRACA; break;
		case e3_2_PODTRZYMANIE_PRZERWA: editParam = Parameter::Nazwa::PODTRZYMANIE_PRZERWA; break;
		case e3_3_OPOZNIENIE_WLACZENIA_DMUCHAWY:	editParam = Parameter::Nazwa::OPOZNIENIE_DMUCHAWY; break;
		case e4_POMPA_CO:				editParam = Parameter::Nazwa::POMPA_CO_TEMP_ZALACZ; break;
		case e5_ALARM_SPADKU_TEMPERATURY:	editParam = Parameter::Nazwa::ALARM_SPADKU_TEMP; break;
		case e6_Dmuchawa:				editParam = Parameter::Nazwa::DMUCHAWA_MOC; break;
		case e7_HISTEREZA_CO:			editParam = Parameter::Nazwa::HISTEREZA_CO_TEMP; break;
		case e8_POMPA_CWU:				editParam = Parameter::Nazwa::POMPA_CWU_TEMP_ZALACZ; break;
		case e8_1_HISTEREZA_CWU:		editParam = Parameter::Nazwa::HISTEREZA_CWU_TEMP; break;
		case e8_2_TRYB_LETNI:			editParam = Parameter::Nazwa::TRYB_LETNI; break;
		case e9_OPCJE_SERWISOWE:		editParam = Parameter::Nazwa::NONE; break;
		case e9_1_USTAWIENIA_FABRYCZNE:	editParam = Parameter::Nazwa::NONE; break;
		case e9_2_POMPA_CWU_AKTYWNA:	editParam = Parameter::Nazwa::AKTYWNA_CWU; break;
		case e9_3_ALARM_TEMP_PODAJNIKA: editParam = Parameter::Nazwa::ALARM_TEMP_PODAJNIKA; break;
		default: 						editParam = Parameter::Nazwa::NONE; break;
		}
		showEkran();
	}

	void goToEdit(Parameter::Nazwa param){
		editParam = param;
		editValue = Parameter::getValue(editParam);
		editMode = true;
		showEkran(editValue);
	}

	void printPattern(const char * pattern, uint32_t value);


// tylko wyswietlenie - w trybie przegladania
	void showEkran(){
		editMode = false;
		uint16_t val = Parameter::getValue(editParam);
		showEkran(val);
	}

	bool edit(Keyboard::Key key);

	void showEkran(uint16_t val);

	void poll();



};

	//	1.Praca ręczna
	//	2.Podajnik opału
	//	 2.1 Podajnik praca(min, 4 sek)
	//	 2.2 Podajnik przerwa(5 min, sek)
	//	 2.3 Podajnik opoznienie(min, 30 sek)
	//	3. Podtrzymanie palenia
	//	 3.1 Podtrzymanie praca ( 3 sek) 1-59
	//	 3.2 Podtrzymanie przerwa ( 29 min )
	//	 3.3 Opoznienie wyłączenia dmuchawy (25 sek)
	//	4. Pompa C.O.
	//	 Temp zal. 40 oC 50-80
	//	5. Alarm spadku temperatury
	//	 1 godz min
	//	6. Dmuchawa
	//	 moc 1-4
	//	7. Histereza
	//	 2 oC (2-9)
	//	8. Pompa CWU
	//	 Temp załączenia 50 oC
	//	 8.1 Histereza 3 oC
	//	 8.2 Tryb letni nieaktywny/aktywny
	//	9. Opcje serwisowe
	//	 9.1 Ustawienia fabryczne
	//	 9.2 Pompa CWU aktywna/nieaktywna
	//	 9.3 Temperatura podajnika alarm 85 oC
#endif /* MENU_H_ */
