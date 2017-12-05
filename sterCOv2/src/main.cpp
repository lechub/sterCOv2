//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx.h"
//#include "diag/Trace.h"

#include "QuickTask.h"
#include "Hardware.h"
#include "Sterownik.h"
#include "Pinout.h"
#include "Keyboard.h"
#include "HMI.h"



// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wunused-parameter"
//#pragma GCC diagnostic ignored "-Wmissing-declarations"
//#pragma GCC diagnostic ignored "-Wreturn-type"



Pinout pins = Pinout();
Keyboard keys = Keyboard(&pins.gpioKey1, &pins.gpioKey2, &pins.gpioKey3, &pins.gpioKey4,
		&pins.gpioImpLeft, &pins.gpioImpRight, &pins.gpioImpPush);
Silnik pompaCo = Silnik(&pins.gpioPompaCO);
Silnik pompaCWU = Silnik(&pins.gpioPompaCWU);
Silnik dmuchawa = Silnik(&pins.gpioDmuchawa);
Silnik podajnik = Silnik(&pins.gpioPodajnik);
ST7032I st7032iDriver = ST7032I();
TextLcd lcd = TextLcd(&st7032iDriver);

Sterownik sterownikCO = Sterownik(&pompaCo, &pompaCWU, &dmuchawa, &podajnik );
//Sterownik * sterCO = &sterownikCO
Sterownik * sterCO = &sterownikCO;
Menu menu = Menu();
//HMI hmi = HMI(&sterownikCO, &keys, &lcd, &menu);


/** Wywolanie metody monitor() */
void static inline keyb_callback(){  keys.co10ms(); }


// periodycznie wykonywana funkcja monitor() opakowana w aku_callback()
QuickTask keybQtsk(QuickTask::QT_PERIODIC, keyb_callback, Keyboard::TIME_PERIOD_KEYB_MS);


//int main (int argc, char* argv[])
void main()
{

	/* Configure the system clock */
	//SystemClock_Config();

	Hardware::init();
	pins.setup();

	st7032iDriver.init();

	HMI * hmi;

	hmi = HMI::getInstance();
	hmi->init(sterCO, &keys, &lcd, &menu);


	hmi->lcd->clrScrHome();

	hmi->lcd->lcd_ON();

	hmi->lcd->setCursorMode(CURSOR::NONE);


	if (!Parameter::initEepromMemory()){
		//---------------------->1234567890123456<
		hmi->lcd->printXY(1,0,	"  BLAD EEPROM!  ");
		while(true){;}
	}

	hmi->lcd->print("abcd");
	Hardware::delayMs(100);
	hmi->lcd->setCursorMode(CURSOR::BLINK);
	Hardware::delayMs(100);
	hmi->lcd->print(" dcba ");
	Hardware::delayMs(100);
	hmi->lcd->gotoXY(1,0);
	Hardware::delayMs(100);
	hmi->lcd->print("123456");
	Hardware::delayMs(100);


	//---------------------->1234567890123456<
	hmi->lcd->printXY(0,0,	"Sterow. pieca CO");
	Hardware::delayMs(1000);
	hmi->lcd->printXY(1,0,	"   wersja 1.0   ");

	Hardware::delayMs(1000);

	//------------------->1234567890123456<
	hmi->lcd->printXY(0,0,"@ Leszek Blacha ");
	Hardware::delayMs(1000);
	hmi->lcd->printXY(1,0,"pazdziernik 2017");

	Hardware::delayMs(1000);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

	//uint8_t pos = 0;

	/*##-3- Start the conversion process and enable interrupt ##################*/
	/* Note: Considering IT occurring after each number of ADC conversions      */
	/*       (IT by DMA end of transfer), select sampling time and ADC clock    */
	/*       with sufficient duration to not create an overhead situation in    */
	/*        IRQHandler. */
//	if(HAL_ADC_Start_DMA(&hadc1, (uint32_t*)Pomiar::getDataTablePtr(), Pomiar::getIloscWejsc()) != HAL_OK)
//	{
//		/* Start Conversation Error */
//		Error_Handler();
//	}

	hmi->menu->goToEkran(Menu::EKRAN::e_AUTOMAT);
	do{
		Hardware::WDOG_Reload();					// przeladowanie Watch-doga
		QuickTask::runQuickTasks();
		//komunikacja();	// bez opoznien

	} while (true);			// do konca swiata

}

//#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
