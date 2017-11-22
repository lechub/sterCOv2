/*
 * Keyboard.h
 *
 *  Created on: 15.09.2016
 *      Author: lechu
 */

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "Button.h"
#include "Fifo.h"

class Keyboard {

public:
	static constexpr uint16_t BUFFERSIZE = 4;	// rozmiar bufora znakow

	static constexpr char KEY_UP		= static_cast<char>(0x0f0);
	static constexpr char KEY_DOWN		= static_cast<char>(0x0f1);
	static constexpr char KEY_LEFT		= static_cast<char>(0x0f2);
	static constexpr char KEY_RIGHT		= static_cast<char>(0x0f3);
	static constexpr char KEY_ENTER		= static_cast<char>(0x0f4);
	static constexpr char KEY_MENU		= static_cast<char>(0x0f5);

	static constexpr char KEY_NONE		= static_cast<char>(0x0fe);


private:
	// bufor znakow - od najstarszego na najmlodszego. Brak znaku to 'KEY_NONE'
	char buffer[BUFFERSIZE];

	/**
	 * kasuje ostatni znak, przesuwa wszystkie o jeden i wpisuje najnowszy na koniec
	 */
	void addToBuffer(char key){
		for (uint16_t i = 0; i < BUFFERSIZE-1; i++){
			buffer[i] = buffer[i+1];
		}
		buffer[BUFFERSIZE - 1] = key;
	}

	char getFromBuffer(){
		char result = KEY_NONE;
		for (uint16_t i = 0; i< BUFFERSIZE; i++){
			result = buffer[i];
			if (result != KEY_NONE){
				buffer[i] = KEY_NONE;
				break;
			}
		}
		return result;
	}


	Gpio gpioRow1 =  Gpio(GPIOA,1);
	Gpio gpioRow2 =  Gpio(GPIOA,1);
	Gpio gpioRow3 =  Gpio(GPIOA,1);
	Gpio gpioRow4 =  Gpio(GPIOA,1);
	Gpio gpioCol1 =  Gpio(GPIOA,1);
	Gpio gpioCol2 =  Gpio(GPIOA,1);
	Gpio gpioCol3 =  Gpio(GPIOA,1);
	Gpio gpioCol4 =  Gpio(GPIOA,1);

	Gpio * row[4] = {&gpioRow1, &gpioRow2, &gpioRow3, &gpioRow4};
	Gpio * col[4] = {&gpioCol1, &gpioCol2, &gpioCol3, &gpioCol4};

	static constexpr char keyTable[4][5]
								  = {
			{'1','2','3','A', KEY_UP},
			{'4','5','6','B', KEY_DOWN},
			{'7','8','9','C', KEY_LEFT},
			{'*','0','#','D', KEY_RIGHT},
	};

	void setRowLo(uint8_t nr){
		for(uint8_t i = 0; i < 4; i++){
			row[i]->pinSet(i == nr);
		}
	}

	void setColLo(uint8_t nr){
		for(uint8_t i = 0; i < 4; i++){
			col[i]->pinSet(i == nr);
		}
	}

	char getPressedKey(){
		for (uint8_t i = 0; i<4; i++){
			setColLo(i);
			for (uint8_t j = 0; j<4; j++){
				if (!row[j]->getInputValue()){
					return keyTable[i][j];
				}
			}
		}
		return KEY_NONE;
	}

	public:
		Keyboard() {
			for (uint16_t i = 0; i<BUFFERSIZE; i++)	buffer[i] = KEY_NONE;
		}

		bool haveBufferedKey() const{
			for (uint16_t i = 0; i<BUFFERSIZE; i++){
				if (buffer[i] != KEY_NONE) return true;
			}
		}

		char getKey(){
			checkKeys();
			return getFromBuffer();
		}

		char getUnbufferedKey(){
			return getPressedKey();
		}

		void checkKeys(){
			char key = getPressedKey();
			if (key == KEY_NONE) return;
			addToBuffer(key);
		}

		//	Keyboard(Button * tableOfKeys, uint16_t ilosc) {
		//		keys = tableOfKeys;
		//		count = ilosc;
		//		for (uint16_t i = 0; i<BUFFERSIZE; i++)	buffer[i] = NONE;
		//	}
		//
		//	bool haveBufferedKey() const{
		//		for (uint16_t i = 0; i<BUFFERSIZE; i++){
		//			if (buffer[i] != NONE) return true;
		//		}
		//	}
		//
		//	Button * getKey(){
		//		for (uint16_t i = 0; i<BUFFERSIZE; i++){
		//
		//		}
		//	}
		//
		//	bool * isPressed(uint16_t keyId){
		//		return keys[0];
		//	}



	};

	//	typedef enum {
	//		NUM0 = 0,
	//		NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9,
	//		UP = 0x0,
	//		DOWN,
	//		LEFT,
	//		RIGHT,
	//		MENU = ,
	//		ENTER,
	//		NONE = 255,
	//	}Key;


	//	Gpio gpioUp =  Gpio(GPIOA,1);
	//	Gpio gpioDown =  Gpio(GPIOA,1);
	//	Gpio gpioLeft =  Gpio(GPIOA,1);
	//	Gpio gpioRight =  Gpio(GPIOA,1);
	//	Gpio gpioMenu =  Gpio(GPIOA,1);
	//	Gpio gpioEnter =  Gpio(GPIOA,1);
	//	Button up = Button((uint32_t)UP, &gpioUp, Button::ActiveType::ACTIVE_LOW);
	//	Button down = Button((uint32_t)DOWN, &gpioDown, Button::ActiveType::ACTIVE_LOW);
	//	Button right = Button((uint32_t)UP, &gpioRight, Button::ActiveType::ACTIVE_LOW);
	//	Button left = Button((uint32_t)DOWN, &gpioLeft, Button::ActiveType::ACTIVE_LOW);
	//	Button enter = Button((uint32_t)ENTER, &gpioEnter, Button::ActiveType::ACTIVE_LOW);
	//	Button menu = Button((uint32_t)MENU, &gpioMenu, Button::ActiveType::ACTIVE_LOW);
	//	Button none = Button((uint32_t)NONE, nullptr, Button::ActiveType::ACTIVE_LOW);

	//	Button keys [] = {
	//		{Button((uint32_t)UP, &gpioUp, Button::ActiveType::ACTIVE_LOW)},
	//		{Button((uint32_t)DOWN, &gpioDown, Button::ActiveType::ACTIVE_LOW)},
	//		{Button((uint32_t)MENU, &gpioMenu, Button::ActiveType::ACTIVE_LOW)},
	//		{Button((uint32_t)ENTER, &gpioEnter, Button::ActiveType::ACTIVE_LOW)},
	//		{Button((uint32_t)NONE, nullptr, Button::ActiveType::ACTIVE_LOW)}
	//	};

	//	Button * keys [5] = { &up, &down, &menu, &enter, &none };
	//	uint16_t count = 4;



#endif /* KEYBOARD_H_ */
