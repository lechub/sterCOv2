/*
 * EditValue.h
 *
 *  Created on: 12.07.2017
 *      Author: lechu
 */

#ifndef EDITVALUE_H_
#define EDITVALUE_H_

#include "Sterownik.h"
#include "Keyboard.h"
#include <string.h>


class EditValue {

public:
	typedef enum{
		WAIT, EDIT, RESULT
	}State;

private:
	static constexpr size_t BUFFLEN = 30;
	uint32_t value;
	size_t len = 0;
	Keyboard * keyb;
	TextLcd * lcd;
	const char *pattern;
	uint16_t cursorPos = 0;
	char bufor [BUFFLEN];
	State state = WAIT;
	bool activeEdit = false;
	bool editNotSlide = false;

private:
	/**
	 * Modyfikuje pole liczbowe we wzorze. Wzór typu "To jest liczba 00:60" oznacza, ze maksymalne cyfry to "99:59"
	 * @param pattern wzor do edycji
	 * @param initValue liczba do edycji/zmiany
	 * @param cursorPos pozycja kursora, na ktorej bedzie zmiana
	 * @param up_not_down zmiana na cyfrę większą lub mniejszą
	 * @return liczba initValue po zmianie, czyli edycji
	 */
	uint32_t editNumbersWithPattern(Keyboard::Key key);


	/**
	 * Edytuje pole liczbowe we wzorze. Wzór typu "To jest liczba 00:60" oznacza, ze maksymalne cyfry to "99:59"
	 * @param pattern wzor do edycji
	 * @param initValue liczba do edycji/zmiany
	 * @param cursorPos pozycja kursora, na ktorej bedzie zmiana
	 * @param up_not_down zmiana na cyfrę większą lub mniejszą
	 * @return liczba initValue po zmianie, czyli edycji
	 */
	//int32_t showNumbersWithPattern(Keyboard::Key key);


	int32_t getOffsetFromPosition(uint16_t position){
		uint16_t tmp = 0;
		for (size_t i = 0; i < len; i++){		// szukanie
			if ( isDigitAscii(pattern[i])){		// szukanie cyfry w pattern
				if (tmp == position) return i;
				tmp++;
			}
		}
		return -1;
	}

	bool incrementPos(){
		if (getOffsetFromPosition((uint16_t)(cursorPos+1)) >= 0){
			cursorPos++;
			return true;
		}
		return false;
	}

	bool decrementPos(){
		if (cursorPos == 0) return false;
		if (getOffsetFromPosition((uint16_t)(cursorPos-1)) >= 0){
			cursorPos--;
			return true;
		}
		return false;
	}


public:
	void set(const char *wzor, uint32_t initValue, Sterownik * sterownik){
		pattern = wzor;
		value = initValue;
		len = strlen(pattern);
		activeEdit = true;
		//lcd = sterownik->lcd;
		//keyb = sterownik->keys;
		state = WAIT;
		editNotSlide = false;
	}

	EditValue(const char *wzor, uint32_t initValue, Sterownik * sterownik){ set(wzor, initValue, sterownik); }

	uint32_t getValue(){ return value; }

	State getState() { return state; }

	State show(Keyboard::Key key){
		//		if (state != )
			switch(key){
			case Keyboard::Key::NONE:
				return state;
			case Keyboard::Key::RIGHT:
				if (editNotSlide){
					editNumbersWithPattern(key);
				}else{
					if (! incrementPos()) state = State::RESULT;
					else editNumbersWithPattern(Keyboard::Key::NONE);
				}
				break;
			case Keyboard::Key::LEFT:
				if (editNotSlide){
					editNumbersWithPattern(key);
				}else{
					if (! decrementPos()) state = State::RESULT;
					else editNumbersWithPattern(Keyboard::Key::NONE);
				}
				break;
			case Keyboard::Key::ENTER:
				editNotSlide = true;
				editNumbersWithPattern(Keyboard::Key::NONE);
				break;
			case Keyboard::Key::CANCEL:
				editNotSlide = false;
			}
			return state;
	}

	State show(){
		Keyboard::Key key = keyb->getKey();
		return show(key);
	}


};	//class

#endif /* EDITVALUE_H_ */
