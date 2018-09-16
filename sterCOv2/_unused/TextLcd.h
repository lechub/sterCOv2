/*
 * TextLcd.h
 *
 *  Created on: 16.09.2016
 *      Author: lechu
 */

#ifndef TEXTLCD_H_
#define TEXTLCD_H_

#include <stdint.h>
#include "st7032i.h"

class TextLcd {

private:
	ST7032I * driver = nullptr;
public:
	static constexpr uint16_t NUMBER_OF_LINES	 = 2;
	static constexpr uint16_t CHARACTER_PER_LINE = 16;

//	typedef enum{
//		HIDE, SHOW, BLINK,
//	}CursorMode;

	TextLcd(){;}

	TextLcd(ST7032I * lcdDriver) {
		init(lcdDriver);
	}

	void init(ST7032I * lcdDriver) {
		driver = lcdDriver;
	}

	void poll(){ driver->poll(); }

	bool gotoXY(uint16_t rowNr, uint16_t columnNr){
		return driver->gotoXY(rowNr,columnNr);
	}
	bool print(char znak){
		return driver->print(znak);
	}
	bool print(const char * str){
		return driver->print(str);
	}
	bool printXY(uint16_t rowNr, uint16_t columnNr, const char * str){
		if (!driver->gotoXY(rowNr, columnNr)) return false;
		return driver->print(str);
	}
	bool printXY(uint16_t rowNr, uint16_t columnNr, char znak){
		if (!driver->gotoXY(rowNr, columnNr)) return false;
		return driver->print(znak);
	}
	bool cursorLeft(){
		return driver->cursorLeft();
	}
	bool cursorRight(){
		return driver->cursorRight();
	}
	bool cursorLeft(uint16_t chars){
		while (chars){
			if (!cursorLeft()) return false;
			chars--;
		}
		return true;
	}
	bool cursorRight(uint16_t chars){
		while (chars){
			if (!cursorRight()) return false;
			chars--;
		}
		return true;
	}
	bool setCursorMode(CURSOR type){
		return driver->cursorSet(type);		// ToDo zmienic na prawdziwe BLINK
	}

	bool home(){
		return driver->homeScreen();
	}

	bool clrScrHome(){
		return driver->clearScreen();
	}

	bool lcd_ON(){
		return driver->lcd_ON();
	}

	bool lcd_OFF(){
		return driver->lcd_OFF();
	}

	bool printWDot(uint32_t value);
	bool printNumbersWithPattern(const char * pattern, const uint32_t initValue);
	bool printNumbersWithPatternSetCursor(const char * pattern, const uint32_t initValue);
	int32_t getOffsetOfLastDigit(const char * pattern);
};


#endif /* TEXTLCD_H_ */
