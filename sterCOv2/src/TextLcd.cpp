/*
 * TextLcd.cpp
 *
 *  Created on: 16.09.2016
 *      Author: lechu
 */

#include "TextLcd.h"
#include <math.h>
#include <ctype.h>
#include "myUtils.h"
#include "st7032i.h"


bool TextLcd::printWDot(uint32_t value){
	if (value > 1000000) value = 0;
	char buf[10];
	uint32_t len = my_itoa(value,buf);
	char underDot = buf[len-2];
	buf[len-2] = '\0';
	print(buf);
	print('.');
	buf[len-2] = underDot;
	print(buf[len-2]);
	return true;
}




/**
 * Wyswietla liczby według wzorca
 * np.pattern= "000:00.00" i initValue=123456 wyswietli "012:34.56"
 */
bool TextLcd::printNumbersWithPattern(const char * pattern, uint32_t initValue){
	char bufor [20];
	my_strcpy(bufor, pattern, true);	// skopiowanie wzoru razem z terminacja
	itoaWithPattern(bufor, initValue);
	print(bufor);
	return true;
}

//bool TextLcd::printNumbersWithPatternSetCursor(const char * pattern, uint32_t initValue){
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


/**
 * Oblicza przesuniecie do ostatniego wystapienia cyfry w tekscie
 * np.pattern= "000:00.00aaa" zwroci 8
 * @return offset do ostatniego wystapienia cyfry lub -1, gdy nie ma cyfr
 */
int32_t TextLcd::getOffsetOfLastDigit(const char * pattern){
	char znak;
	int32_t offset = 0;
	int32_t result = -1;
	do{
		znak = pattern[offset];
		if (isdigit(znak-'0')) result = offset;
		offset++;
	}while (znak != '\0');
	return result;
}




//bool TextLcd::printNumbersWithPattern(const char * const pattern, uint32_t initValue){
//	constexpr char patternChar = '0';
//	uint32_t iloscZnakow = 0;
//	uint32_t iloscCyfr = 0;
//	const char * txt = pattern;
//	while (*txt != '\0'){
//		if (*txt == patternChar) iloscCyfr++;
//		iloscZnakow++;
//		txt++;
//	}
////	uint32_t val = initValue;
////	uint32_t tmp = power10(iloscCyfr);
//	if ((power10(iloscCyfr)-1) < initValue) initValue = 1; // jesli sie nie miesci to ==1
//	txt = pattern;
//	while(*txt != '\0'){
//		if (*txt == patternChar){
//			uint32_t nr = power10(iloscCyfr-1);
//			uint8_t cyfra = initValue / nr;
//			initValue %= nr;
//			print(cyfra + '0');
//		}else{
//			print(*txt);
//		}
//		txt++;
//	}
//	return true;
//}


