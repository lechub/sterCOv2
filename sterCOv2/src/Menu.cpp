/*
 * Menu.cpp
 *
 *  Created on: 18 paź 2017
 *      Author: lechu
 */

#include "Menu.h"

#include <ctype.h>
#include "Keyboard.h"
#include "TextLcd.h"
#include "HMI.h"
#include "Parameter.h"

void Menu::poll(){
	Keyboard::Key key = keys->getKey();
	if (key == Keyboard::Key::NONE) return;



//		//		//********** DEBUG ************
//		//
//		//		Keyboard::Key key = keys.getKey();
//				static uint8_t pos = 0;
//				pos++;
//				if (pos > 5) pos = 0;
//
//				switch(key){
//				case Keyboard::Key::ENTER: lcd->printXY(1,pos," Enter "); break;
//				case Keyboard::Key::CANCEL: lcd->printXY(1,pos," Cancel "); break;
//				case Keyboard::Key::LEFT: lcd->printXY(1,pos," Left "); break;
//				case Keyboard::Key::RIGHT: lcd->printXY(1,pos," Right "); break;
//				default: break;
//				}
//				return;
//		//
//		//
//		//
//		//		//*****************************

	if (editMode){
		edit(key);
		return;
	}

	// odrzucanie CANCEL z impulsatora  -zeby nie wychodzil od razu z opcji menu
	static bool enterToMem = false;
	if ((enterToMem)&&(key == Keyboard::Key::CANCEL)){
		enterToMem = false;
		return;
	}
	enterToMem = (key == Keyboard::Key::ENTER);

	switch(ekran){
	case e_AUTOMAT:
	{
		switch (key){
		case Keyboard::Key::ENTER:	goToEdit(Parameter::Nazwa::TEMPERATURA_CO); break;
		case Keyboard::Key::RIGHT:	goToEkran(EKRAN::e1_PRACA_RECZNA); break;
		case Keyboard::Key::LEFT: 	goToEkran(EKRAN::e9_OPCJE_SERWISOWE); break;
		case Keyboard::Key::CANCEL:
		default:					goToEkran(EKRAN::e_AUTOMAT); break;
		}
	}break;
	case e1_PRACA_RECZNA:
	{
		switch (key){
		case Keyboard::Key::ENTER:
			praca->setManualMode(!praca->isInManualMode());
			goToEkran(EKRAN::e1_PRACA_RECZNA);
			break;
		case Keyboard::Key::RIGHT:	goToEkran(EKRAN::e2_PODAJNIK_OPALU); break;
		case Keyboard::Key::LEFT: 	goToEkran(EKRAN::e9_OPCJE_SERWISOWE); break;
		case Keyboard::Key::CANCEL:
		default:					goToEkran(EKRAN::e_AUTOMAT); break;
		}
	}break;
	case e2_PODAJNIK_OPALU:
	{
		switch (key){
		case Keyboard::Key::ENTER:	goToEkran(EKRAN::e2_1_PODAJNIK_PRACA); break;
		case Keyboard::Key::RIGHT:	goToEkran(EKRAN::e3_PODTRZYMANIE_PALENIA); break;
		case Keyboard::Key::LEFT: 	goToEkran(EKRAN::e1_PRACA_RECZNA); break;
		case Keyboard::Key::CANCEL:
		default:					goToEkran(EKRAN::e_AUTOMAT); break;
		}
	}break;
	case e2_1_PODAJNIK_PRACA:
	{
		switch (key){
		case Keyboard::Key::ENTER:	goToEdit(Parameter::Nazwa::PODAJNIK_PRACA); break;
		case Keyboard::Key::RIGHT:	goToEkran(EKRAN::e2_2_PODAJNIK_PRZERWA); break;
		case Keyboard::Key::LEFT: 	goToEkran(EKRAN::e2_3_PODAJNIK_OPOZNIENIE); break;
		case Keyboard::Key::CANCEL:
		default:					goToEkran(EKRAN::e2_PODAJNIK_OPALU); break;
		}
	}break;
	case e2_2_PODAJNIK_PRZERWA:
	{
		switch (key){
		case Keyboard::Key::ENTER:	goToEdit(Parameter::PODAJNIK_PRZERWA);  break;	//(5 min:  break; sek):  ;
		case Keyboard::Key::RIGHT:	goToEkran(EKRAN::e2_3_PODAJNIK_OPOZNIENIE); break;
		case Keyboard::Key::LEFT: 	goToEkran(EKRAN::e2_1_PODAJNIK_PRACA); break;
		case Keyboard::Key::CANCEL:
		default:					goToEkran(EKRAN::e2_PODAJNIK_OPALU); break;
		}
	}break;
	case e2_3_PODAJNIK_OPOZNIENIE:
	{
		switch (key){
		case Keyboard::Key::ENTER:	goToEdit(Parameter::PODAJNIK_OPOZNIENIE); break;  //(min:  break; 30 sek):  ;
		case Keyboard::Key::RIGHT:	goToEkran(EKRAN::e2_1_PODAJNIK_PRACA); break;
		case Keyboard::Key::LEFT: 	goToEkran(EKRAN::e2_2_PODAJNIK_PRZERWA); break;
		case Keyboard::Key::CANCEL:
		default:					goToEkran(EKRAN::e2_PODAJNIK_OPALU); break;
		}
	}break;
	case e3_PODTRZYMANIE_PALENIA:
	{
		switch (key){
		case Keyboard::Key::ENTER:	goToEkran(EKRAN::e3_1_PODTRZYMANIE_PRACA); break;  //(min:  break; 30 sek):  ;
		case Keyboard::Key::RIGHT:	goToEkran(EKRAN::e4_POMPA_CO); break;
		case Keyboard::Key::LEFT: 	goToEkran(EKRAN::e2_PODAJNIK_OPALU); break;
		case Keyboard::Key::CANCEL:
		default:					goToEkran(EKRAN::e_AUTOMAT); break;
		}
	}break;
	case e3_1_PODTRZYMANIE_PRACA:
	{
		switch (key){
		case Keyboard::Key::ENTER:	goToEdit(Parameter::PODTRZYMANIE_PRACA); break;  // ( 3 sek) 1-59:  ;
		case Keyboard::Key::RIGHT:	goToEkran(EKRAN::e3_2_PODTRZYMANIE_PRZERWA); break;
		case Keyboard::Key::LEFT: 	goToEkran(EKRAN::e3_3_OPOZNIENIE_WLACZENIA_DMUCHAWY); break;
		case Keyboard::Key::CANCEL:
		default:					goToEkran(EKRAN::e3_PODTRZYMANIE_PALENIA); break;
		}
	}break;
	case e3_2_PODTRZYMANIE_PRZERWA:
	{
		switch (key){
		case Keyboard::Key::ENTER:	goToEdit(Parameter::PODTRZYMANIE_PRZERWA); break;  //( 29 min ):  ;
		case Keyboard::Key::RIGHT:	goToEkran(EKRAN::e3_3_OPOZNIENIE_WLACZENIA_DMUCHAWY); break;
		case Keyboard::Key::LEFT: 	goToEkran(EKRAN::e3_1_PODTRZYMANIE_PRACA); break;
		case Keyboard::Key::CANCEL:
		default:					goToEkran(EKRAN::e3_PODTRZYMANIE_PALENIA); break;
		}
	}break;
	case e3_3_OPOZNIENIE_WLACZENIA_DMUCHAWY:
	{
		switch (key){
		case Keyboard::Key::ENTER:	goToEdit(Parameter::OPOZNIENIE_DMUCHAWY); break;  //(25 sek):  ;
		case Keyboard::Key::RIGHT:	goToEkran(EKRAN::e3_1_PODTRZYMANIE_PRACA); break;
		case Keyboard::Key::LEFT: 	goToEkran(EKRAN::e3_2_PODTRZYMANIE_PRZERWA); break;
		case Keyboard::Key::CANCEL:
		default:					goToEkran(EKRAN::e3_PODTRZYMANIE_PALENIA); break;
		}
	}break;
	case e4_POMPA_CO:
	{
		switch (key){
		case Keyboard::Key::ENTER:	goToEdit(Parameter::POMPA_CO_TEMP_ZALACZ); break; 	//Temp zal. 40 oC 50-80:  ;
		case Keyboard::Key::RIGHT:	goToEkran(EKRAN::e5_ALARM_SPADKU_TEMPERATURY); break;
		case Keyboard::Key::LEFT: 	goToEkran(EKRAN::e3_PODTRZYMANIE_PALENIA); break;
		case Keyboard::Key::CANCEL:
		default:					goToEkran(EKRAN::e_AUTOMAT); break;
		}
	}break;
	case e5_ALARM_SPADKU_TEMPERATURY:
	{
		switch (key){
		case Keyboard::Key::ENTER:	goToEdit(Parameter::Nazwa::ALARM_SPADKU_TEMP); break; //:  ; godz min:  ;
		case Keyboard::Key::RIGHT:	goToEkran(EKRAN::e6_Dmuchawa); break;
		case Keyboard::Key::LEFT: 	goToEkran(EKRAN::e4_POMPA_CO); break;
		case Keyboard::Key::CANCEL:
		default:					goToEkran(EKRAN::e_AUTOMAT); break;
		}
	}break;
	case e6_Dmuchawa:
	{
		switch (key){
		case Keyboard::Key::ENTER:	goToEdit(Parameter::Nazwa::DMUCHAWA_MOC); break;  //moc 1-4:
		case Keyboard::Key::RIGHT:	goToEkran(EKRAN::e7_HISTEREZA_CO); break;
		case Keyboard::Key::LEFT: 	goToEkran(EKRAN::e5_ALARM_SPADKU_TEMPERATURY); break;
		case Keyboard::Key::CANCEL:
		default:					goToEkran(EKRAN::e_AUTOMAT); break;
		}
	}break;
	case e7_HISTEREZA_CO:
	{
		switch (key){
		case Keyboard::Key::ENTER:	goToEdit(Parameter::Nazwa::HISTEREZA_CO_TEMP); break; 	//2 oC (2-9):
		case Keyboard::Key::RIGHT:	goToEkran(EKRAN::e8_POMPA_CWU); break;
		case Keyboard::Key::LEFT: 	goToEkran(EKRAN::e6_Dmuchawa); break;
		case Keyboard::Key::CANCEL:
		default:					goToEkran(EKRAN::e_AUTOMAT); break;
		}
	}break;
	case e8_POMPA_CWU:
	{
		switch (key){
		case Keyboard::Key::ENTER:	goToEdit(Parameter::Nazwa::POMPA_CWU_TEMP_ZALACZ); break;  	//Temp załączenia 50 oC:
		case Keyboard::Key::RIGHT:	goToEkran(EKRAN::e9_OPCJE_SERWISOWE); break;
		case Keyboard::Key::LEFT: 	goToEkran(EKRAN::e7_HISTEREZA_CO); break;
		case Keyboard::Key::CANCEL:
		default:					goToEkran(EKRAN::e_AUTOMAT); break;
		}
	}break;
	case e8_1_HISTEREZA_CWU:
	{
		switch (key){
		case Keyboard::Key::ENTER:	goToEdit(Parameter::HISTEREZA_CWU_TEMP); break;  //(25 sek):  ; break; //3 oC:  break;
		case Keyboard::Key::RIGHT:	goToEkran(EKRAN::e8_2_TRYB_LETNI); break;
		case Keyboard::Key::LEFT: 	goToEkran(EKRAN::e8_2_TRYB_LETNI); break;
		case Keyboard::Key::CANCEL:
		default:					goToEkran(EKRAN::e8_POMPA_CWU); break;
		}
	}break;
	case e8_2_TRYB_LETNI:
	{
		switch (key){
		case Keyboard::Key::ENTER:
		{
			Parameter::SEZON sezon = Parameter::SEZON(Parameter::getValue(Parameter::Nazwa::TRYB_LETNI));
			Parameter::setValue(Parameter::Nazwa::TRYB_LETNI, (sezon != Parameter::SEZON::ZIMA) ? Parameter::SEZON::ZIMA : Parameter::SEZON::LATO );
			praca->setTrybLetni(sezon);
			goToEkran(EKRAN::e8_2_TRYB_LETNI);
		}break;
		case Keyboard::Key::RIGHT:	goToEkran(EKRAN::e8_1_HISTEREZA_CWU); break;
		case Keyboard::Key::LEFT: 	goToEkran(EKRAN::e8_1_HISTEREZA_CWU); break;
		case Keyboard::Key::CANCEL:
		default:					goToEkran(EKRAN::e8_POMPA_CWU); break;
		}
	}break;
	case e9_OPCJE_SERWISOWE:
	{
		switch (key){
		case Keyboard::Key::ENTER:	goToEkran(EKRAN::e9_1_USTAWIENIA_FABRYCZNE); break;  //(min:  break; 30 sek):  ;
		case Keyboard::Key::RIGHT:	goToEkran(EKRAN::e1_PRACA_RECZNA); break;
		case Keyboard::Key::LEFT: 	goToEkran(EKRAN::e8_POMPA_CWU); break;
		case Keyboard::Key::CANCEL:
		default:					goToEkran(EKRAN::e_AUTOMAT); break;
		}
	}break;
	case e9_1_USTAWIENIA_FABRYCZNE:
	{
		switch (key){
		case Keyboard::Key::ENTER:
			Parameter::setDefaultValues();
			lcd->printXY(1,0, "  ZRESETOWANE!  ");
			break;
		case Keyboard::Key::RIGHT:	goToEkran(EKRAN::e9_2_POMPA_CWU_AKTYWNA); break;
		case Keyboard::Key::LEFT: 	goToEkran(EKRAN::e9_3_ALARM_TEMP_PODAJNIKA); break;
		case Keyboard::Key::CANCEL:
		default:					goToEkran(EKRAN::e9_OPCJE_SERWISOWE); break;
		}
	}break;
	case e9_2_POMPA_CWU_AKTYWNA:
	{
		switch (key){
		case Keyboard::Key::ENTER:
		{
			bool aktywnaCWU = Parameter::getValue(Parameter::Nazwa::AKTYWNA_CWU); // aktywna/nieaktywna:
			Parameter::setValue(Parameter::Nazwa::AKTYWNA_CWU, !aktywnaCWU);
			goToEkran(EKRAN::e9_2_POMPA_CWU_AKTYWNA);
		}break;
		case Keyboard::Key::RIGHT:	goToEkran(EKRAN::e9_3_ALARM_TEMP_PODAJNIKA); break;
		case Keyboard::Key::LEFT: 	goToEkran(EKRAN::e9_1_USTAWIENIA_FABRYCZNE); break;
		case Keyboard::Key::CANCEL:
		default:					goToEkran(EKRAN::e9_OPCJE_SERWISOWE); break;
		}
	}break;
	case e9_3_ALARM_TEMP_PODAJNIKA: // alarm 85 oC
	{
		switch (key){
		case Keyboard::Key::ENTER:	goToEdit(Parameter::Nazwa::ALARM_TEMP_PODAJNIKA); break;
		case Keyboard::Key::RIGHT:	goToEkran(EKRAN::e9_1_USTAWIENIA_FABRYCZNE); break;
		case Keyboard::Key::LEFT: 	goToEkran(EKRAN::e9_2_POMPA_CWU_AKTYWNA); break;
		case Keyboard::Key::CANCEL:
		default:					goToEkran(EKRAN::e9_OPCJE_SERWISOWE); break;
		}
	}break;
	default: goToEkran(EKRAN::e_AUTOMAT); break;
	}
}

//bool printNumbersWithPatternSetCursor(const char * pattern, uint32_t initValue){
//	printNumbersWithPattern(pattern, initValue);
//	const char * ptr = pattern;
//	while (*ptr++ != '\0'){;}
//	char znak;
//	uint32_t offset = 0;
//	do{
//		znak = char(*ptr - '0');
//		if (isdigit(znak)) break;
//		ptr--;
//		offset++;
//	}while (ptr >= pattern);
//	cursorLeft(offset);
//	setCursorMode(CURSOR::BLINK);
//	return true;
//}

void Menu::printPattern(const char * pattern, uint32_t value){
	lcd->printNumbersWithPattern(pattern, value);
	if (editMode){
		const char * ptr = pattern;
		while (*ptr != '\0'){ptr++;}
		char znak;
		uint16_t offset = 0;
		do{
			znak = char(*ptr);
			if (isdigit(znak)) break;
			ptr--;
			offset++;
		}while (ptr >= pattern);
		lcd->cursorLeft(offset);
		lcd->setCursorMode(CURSOR::BLINK);
	}else{
		lcd->setCursorMode(CURSOR::NONE);
	}
}


void Menu::showEkran(uint16_t val){

	Parameter::Nazwa param = Parameter::Nazwa::NONE;

	switch(ekran){
	case e_AUTOMAT:///------->1234567890123456<
	{
		uint32_t tpiec = ster->getTempCO();
		param = Parameter::Nazwa::TEMPERATURA_CO;
		const char * patt = Parameter::getParamPattern(param);
		lcd->gotoXY(0,0);
		//--------------------------->1234567890123456<
		lcd->printNumbersWithPattern("T. pieca:00.0 oC",tpiec);
		lcd->gotoXY(1,0);
		printPattern(patt,val);
	}break;
	case e1_PRACA_RECZNA://->1234567890123456<
	{
		lcd->printXY(0,0,	"1. Tryb pracy:  ");
		if (praca->isInManualMode()){
			lcd->printXY(1,0,"   >MANUAL<     ");
		}else{//------------->1234567890123456<
			lcd->printXY(1,0,"   >AUTOMAT<    ");
		}
	}break;
	case e2_PODAJNIK_OPALU:
	{//------------------>1234567890123456<
		lcd->printXY(0,0,"2.Podajnik opalu");
		lcd->printXY(1,0,"      >>        ");
	}break;
	case e2_1_PODAJNIK_PRACA:
	{//--------------------->1234567890123456<
		lcd->printXY(0,0,	"2.1.Podaj. praca");
		lcd->gotoXY(1,0);
		param = Parameter::Nazwa::PODAJNIK_PRACA;
		const char * patt = Parameter::getParamPattern(param);
		printPattern(patt, val);
	}break;
	case e2_2_PODAJNIK_PRZERWA:  	//(5 min:  break; sek):  ;
	{//--------------------->1234567890123456<
		lcd->printXY(0,0,	"2.2.Pod. przerwa");
		lcd->gotoXY(1,0);
		param = Parameter::Nazwa::PODAJNIK_PRZERWA;
		const char * patt = Parameter::getParamPattern(param);
		printPattern(patt, val);
	}break;
	case e2_3_PODAJNIK_OPOZNIENIE:   //(min:  break; 30 sek):  ;
	{//--------------------->1234567890123456<
		lcd->printXY(0,0,	"2.3.Pod. opozn. ");
		lcd->gotoXY(1,0);
		param = Parameter::Nazwa::PODAJNIK_OPOZNIENIE;
		const char * patt = Parameter::getParamPattern(param);
		printPattern(patt, val);
	}break;
	case e3_PODTRZYMANIE_PALENIA:
	{//------------------>1234567890123456<
		lcd->printXY(0,0,"3.Podt. palenia ");
		lcd->printXY(1,0,"      >>        ");
	}break;
	case e3_1_PODTRZYMANIE_PRACA:   // ( 3 sek) 1-59:  ;
	{//--------------------->1234567890123456<
		lcd->printXY(0,0,	"3.1.Podt. praca ");
		lcd->gotoXY(1,0);
		param = Parameter::Nazwa::PODTRZYMANIE_PRACA;
		const char * patt = Parameter::getParamPattern(param);
		printPattern(patt, val);

	}break;
	case e3_2_PODTRZYMANIE_PRZERWA:   //( 29 min ):  ;
	{//--------------------->1234567890123456<
		lcd->printXY(0,0,	"3.2.Podt. przer.");
		lcd->gotoXY(1,0);
		param = Parameter::Nazwa::PODTRZYMANIE_PRZERWA;
		const char * patt = Parameter::getParamPattern(param);
		printPattern(patt, val);
	}break;
	case e3_3_OPOZNIENIE_WLACZENIA_DMUCHAWY:   //(25 sek):  ;
	{//--------------------->1234567890123456<
		lcd->printXY(0,0,	"3.3.Opozn. dmuch");
		lcd->gotoXY(1,0);
		param = Parameter::Nazwa::PODTRZYMANIE_PRACA;
		const char * patt = Parameter::getParamPattern(param);
		printPattern(patt, val);
	}break;
	case e4_POMPA_CO:  		//Temp zal. 40 oC 50-80:  ;
	{//--------------------->1234567890123456<
		lcd->printXY(0,0,	"4. Pompa C.O.   ");
		lcd->gotoXY(1,0);
		param = Parameter::Nazwa::POMPA_CO_TEMP_ZALACZ;
		const char * patt = Parameter::getParamPattern(param);
		printPattern(patt, val);
	}break;
	case e5_ALARM_SPADKU_TEMPERATURY:   //:  ; godz min:  ;
	{//--------------------->1234567890123456<
		lcd->printXY(0,0,	"5. Alarm spadku ");
		lcd->gotoXY(1,0);
		param = Parameter::Nazwa::ALARM_SPADKU_TEMP;
		const char * patt = Parameter::getParamPattern(param);
		printPattern(patt, val);
	}break;
	case e6_Dmuchawa:  			//moc 1-4:
	{//---------------------->1234567890123456<
		lcd->printXY(0,0,    "6.Dmuchawa moc: ");
		lcd->gotoXY(1,0);
		param = Parameter::Nazwa::DMUCHAWA_MOC;
		const char * patt = Parameter::getParamPattern(param);
		printPattern(patt, val);
	}break;
	case e7_HISTEREZA_CO:  		//2 oC (2-9):
	{//---------------------->1234567890123456<
		lcd->printXY(0,0,    "7.Histereza C.O.");
		lcd->gotoXY(1,0);
		param = Parameter::Nazwa::HISTEREZA_CO_TEMP;
		const char * patt = Parameter::getParamPattern(param);
		printPattern(patt, val);
	}break;
	case e8_POMPA_CWU:  	//Temp załączenia 50 oC:
	{//---------------------->1234567890123456<
		lcd->printXY(0,0,    "8. Pompa C.W.U. ");
		lcd->gotoXY(1,0);
		param = Parameter::Nazwa::POMPA_CWU_TEMP_ZALACZ;
		const char * patt = Parameter::getParamPattern(param);
		printPattern(patt, val);
	}break;
	case e8_1_HISTEREZA_CWU:   //3 oC:  break;
	{//---------------------->1234567890123456<
		lcd->printXY(0,0,    "8.1.Hister. CWU ");
		lcd->gotoXY(1,0);
		param = Parameter::Nazwa::HISTEREZA_CWU_TEMP;
		const char * patt = Parameter::getParamPattern(param);
		printPattern(patt, val);
	}break;
	case e8_2_TRYB_LETNI:   //nieaktywny/aktywny
	{//---------------------->1234567890123456<
		lcd->printXY(0,0,    "8.2 Sezon/tryb  ");
		Parameter::SEZON val = (Parameter::SEZON)Parameter::getValue(Parameter::Nazwa::TRYB_LETNI);
		if (val == Parameter::SEZON::LATO){
			lcd->printXY(1,0,"     LETNI      ");
		}else{
			lcd->printXY(1,0,"    ZIMOWY      ");
		}
	}break;
	case e9_OPCJE_SERWISOWE:
	{//------------------>1234567890123456<
		lcd->printXY(0,0,"9. Opcje serwis.");
		lcd->printXY(1,0,"      >>        ");
	}break;
	case e9_1_USTAWIENIA_FABRYCZNE:
	{//------------------>1234567890123456<
		lcd->printXY(0,0,"9.1.Ustaw. fabr.");
		lcd->printXY(1,0,"      >>        ");
	}break;
	case e9_2_POMPA_CWU_AKTYWNA:   // aktywna/nieaktywna:
	{//---------------------->1234567890123456<
		lcd->printXY(0,0,    "9.2. Pompa CWU  ");
		if (Parameter::getValue(Parameter::Nazwa::AKTYWNA_CWU)){
			lcd->printXY(1,0,"    aktywna     ");
		}else{
			lcd->printXY(1,0,"   nieaktywna   ");
		}
	}break;
	case e9_3_ALARM_TEMP_PODAJNIKA:  // alarm 85 oC
	{//---------------------->1234567890123456<
		lcd->printXY(0,0,    "9.3.Alarm podaj.");
		lcd->gotoXY(1,0);
		param = Parameter::Nazwa::ALARM_TEMP_PODAJNIKA;
		const char * patt = Parameter::getParamPattern(param);
		printPattern(patt, val);
	}break;
	default:
		break;
	}
}

bool Menu::edit(Keyboard::Key key){
	switch(editParam){
	case Parameter::Nazwa::PODAJNIK_PRACA:
	case Parameter::Nazwa::PODTRZYMANIE_PRACA:
	case Parameter::Nazwa::PODAJNIK_OPOZNIENIE:
	case Parameter::Nazwa::OPOZNIENIE_DMUCHAWY:
	case Parameter::Nazwa::TEMPERATURA_CO:
	case Parameter::Nazwa::HISTEREZA_CO_TEMP:
	case Parameter::Nazwa::HISTEREZA_CWU_TEMP:
	case Parameter::Nazwa::POMPA_CO_TEMP_ZALACZ:
	case Parameter::Nazwa::POMPA_CWU_TEMP_ZALACZ:
	case Parameter::Nazwa::ALARM_SPADKU_TEMP:
	case Parameter::Nazwa::ALARM_TEMP_PODAJNIKA:
	{
		switch(key){
		case Keyboard::Key::RIGHT:		editValue += 2;	break;
		case Keyboard::Key::LEFT:		if (editValue >= 2) editValue -= 2; else editValue = 0; break;
		case Keyboard::Key::CANCEL:
		case Keyboard::Key::ENTER:		Parameter::setValue(editParam, editValue); editMode = false; break;
		default:
			break;
		}
		showEkran(editValue);
	}break;
	case Parameter::Nazwa::PODAJNIK_PRZERWA:
	case Parameter::Nazwa::PODTRZYMANIE_PRZERWA:
	{
		switch(key){
		case Keyboard::Key::RIGHT:		editValue += 10;	break;
		case Keyboard::Key::LEFT:		if (editValue >= 10) editValue -= 10; else editValue = 0; break;
		case Keyboard::Key::CANCEL:
		case Keyboard::Key::ENTER:		Parameter::setValue(editParam, editValue);  editMode = false; break;
		default:
			break;
		}
		showEkran(editValue);
	}break;

	case Parameter::Nazwa::DMUCHAWA_MOC:
	{
		switch(key){
		case Keyboard::Key::RIGHT:		editValue++;	break;
		case Keyboard::Key::LEFT:		if (editValue > 0) editValue--; break;
		case Keyboard::Key::CANCEL:
		case Keyboard::Key::ENTER:		Parameter::setValue(editParam, editValue); editMode = false;  break;
		default:
			break;
		}
		showEkran(editValue);
	}break;

	case Parameter::Nazwa::USTAWIENIA_FABRYCZNE:
	case Parameter::Nazwa::MODE_AUTO_MANUAL:
	case Parameter::Nazwa::TRYB_LETNI:
	case Parameter::Nazwa::AKTYWNA_CWU:
	case Parameter::Nazwa::NONE:
	case Parameter::Nazwa::LAST_PARAMETER:
	default:
		editMode = false;
		break;
	}
	return false;
}

