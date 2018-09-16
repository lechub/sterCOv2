/*
 * EditValue.cpp
 *
 *  Created on: 12.07.2017
 *      Author: lechu
 */

#include "EditValue.h"

	uint32_t EditValue::editNumbersWithPattern(Keyboard::Key key){

		if (len >= BUFFLEN) return value;	// za maly bufor dla tej operacji
		my_strcpy(bufor, pattern, true);		// skopiowane do ramu
		itoaWithPattern(bufor, value);		// uzupelnienie cyfr wg. initValue
		uint16_t digitNr = 0;					// numer cyfry do edycji - zaczynamy od pierwszej=0
		size_t digitOffset = len;				// przesuniecie dla tej cyfry w pattern
		for (size_t i = 0; i < len; i++){		// szukanie
			if ( isDigitAscii(pattern[i])){		// szukanie cyfry w pattern
				if(digitNr == cursorPos){		// jesli to wlasciwa pozycja kursora
					digitOffset = i;			// zapamietanie offsetu dla kursora
					switch(key){
					case Keyboard::Key::RIGHT:
					{ // up

						uint8_t znak = bufor[i];
						znak++;						//
						if (znak >= pattern[cursorPos]){
							if ((pattern[cursorPos] == '0')&&(znak > '9')) znak = '0';
							else znak = '0';
						}
						bufor[cursorPos] = znak;
						break;
					}
					case Keyboard::Key::LEFT:
					{
						uint8_t znak = bufor[cursorPos];
						znak--;
						if (znak < '0'){
							if (pattern[cursorPos] == '0') znak = '9';
							else znak = uint8_t (pattern[cursorPos]-1);
						}
						bufor[cursorPos] = znak;
						break;
					}
					default:
						break;
					}
				}else{
					digitNr++;		// inkrementacja numeru cyfry
				}
			}
			lcd->print(bufor[i]);
		}// for
		lcd->cursorLeft(len - digitOffset);
		lcd->cursorMode(FrameBuffer::CursorMode::BLINK); // wlacz mruganie

		//		lcd->setCursorMode(TextLcd::CursorMode::HIDE);		//  schowanie kursora

		uint32_t retValue = 0;
		for (size_t i = 0; i < len; i++){		// przeliczenie wyniku
			if (isDigitAscii(pattern[i])){
				retValue *= 10;
				retValue += (bufor[i] - '0');
			}
		}
		value = retValue;
		return value;
	}


//	/**
//	 * Edytuje pole liczbowe we wzorze. Wzór typu "To jest liczba 00:60" oznacza, ze maksymalne cyfry to "99:59"
//	 * @param pattern wzor do edycji
//	 * @param initValue liczba do edycji/zmiany
//	 * @param cursorPos pozycja kursora, na ktorej bedzie zmiana
//	 * @param up_not_down zmiana na cyfrę większą lub mniejszą
//	 * @return liczba initValue po zmianie, czyli edycji
//	 */
//	int32_t EditValue::showNumbersWithPattern(Keyboard::Key key){
//		static bool editState = false;
//		static uint16_t cursorPos = 0;
//		if (!editState){	// wejscie do edycji
//			size_t len = strlen(pattern);
//			for (size_t i = 0; i < len; i++){		// szukanie
//				if ( isDigitAscii(pattern[i])){		// szukanie cyfry w pattern
//					editState = true;
//					cursorPos = 0;					// ustalenie pierwszej cyfry do edycji
//					break;
//				}
//			}
//			//			editState = false;
//			key = Keyboard::Key::NONE;
//		}else{
//
//		}
//		initValue = editNumbersWithPattern(key);
//
//		//size_t len = strlen(pattern);
//		if (cursorPos);
//		//	editNumbersWithPattern
//	}
