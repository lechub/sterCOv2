/*
 * HMI.h
 *
 *  Created on: 16.09.2016
 *      Author: lechu
 */

#ifndef HMI_H_
#define HMI_H_

#include "Sterownik.h"
#include <string.h>
#include "Keyboard.h"
#include "Menu.h"

class HMI {
public:
	static constexpr uint32_t CZAS_WYJSCIA_Z_MENU_MS = 30 * 1000;	// pol minuty
	static constexpr uint32_t TIME_PERIOD_MONITOR_MS = 5;

//	typedef enum{
//		START,		// wlaczenie - nic nie wyswietla
//		WINIETA,	// wyswietlenie informacji o wersji
//		AUTOMAT,	// wyswietlanie aktualnych danych - temp CO itp.
//		MENU,		// operowanie na menu
//	}Mode;
//



private:
	static constexpr uint8_t TEMP_UST_ROW = 0;
	static constexpr uint8_t TEMP_UST_COL = 8;
	static constexpr uint8_t TEMP_AKT_ROW = 1;
	static constexpr uint8_t TEMP_AKT_COL = 8;
	bool startDelay = true;

public:
	Sterownik * ster;
	TextLcd * lcd;
	Praca * praca;
	Keyboard * keyboard;
	Menu * menu;

	HMI(){;}

	HMI(Sterownik * psterownik, Keyboard * pkeys, TextLcd * plcd, Menu * pmenu){
		init(psterownik, pkeys, plcd, pmenu);
	}

	void init(Sterownik * sterownik, Keyboard * pkeys, TextLcd * plcd, Menu * pmenu) {
		ster = sterownik;
		praca = ster->getPraca();
		lcd = plcd;
		keyboard = pkeys;
		menu = pmenu;
		menu->init(ster, keyboard, lcd);
		startDelay = false;
	}



	void monitor(){
		Hardware::i2cTimerJob();
		menu->poll();
	}

	static HMI * getInstance();


};	// class HMI



#endif /* HMI_H_ */
