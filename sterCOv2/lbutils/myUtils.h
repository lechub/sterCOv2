/*
 * myUtils.h
 *
 *  Created on: 14-02-2013
 *      Author: lechu
 */

#ifndef MYUTILS_H_
#define MYUTILS_H_

#include <stdint.h>
#include <stdbool.h>

//----------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//----------------------------------------------------

/* ***************************************************
 *	@brief Konwerter Integer do Ascii.
 *	Dziala rowniez dla liczb ujemnych.
 *	Nalezy pamietac o odpowiednio duzym buforze:
 *	- dla liczb 8-bitowych - min. 4 bajty
 *	- dla liczb 16-bitowych - min. 6 bajtow
 *	- dla liczb 32-bitowych - min. 21 bajtow
 *	@param val	Liczba do konwersji
 *	@param buf	Wskaznik do bufora wynikowego
 *	@return		Ilosc zajetych znakow w buforze, nie liczac terminujacego zera.
 * */
uint16_t my_itoa(int32_t val, char *buf);

void my_htoa(uint32_t val, char *buf);

void my_btoa(uint8_t val,char *buf);

uint32_t power10(uint32_t power);

bool itoaWithPattern(char * const pattern, uint32_t initValue);

//uint8_t AsciiBcdToByte(uint8_t bcdLo);

/* ***************************************************
 *	@brief Konwertuje  znak Ascii - liczbe szesnastkowa - do jej wartosci binarnej
 *	@param bcd	Znak do konwersji
 *	@return		Liczba po konwersji z zakresu 0..15, lub 0xff, gdy znak wejsciowy nie byl prawidlowa cyfra BCD
 * */
static inline uint8_t AsciiBcdToValue(uint8_t bcd){
	if ((bcd >= '0')&&(bcd <= '9')) bcd = (uint8_t)(bcd -'0');
	else if ((bcd >= 'A')&&(bcd <= 'F')) bcd = (uint8_t)((bcd -'A') + 10);
	else if ((bcd >= 'a')&&(bcd <= 'f')) bcd = (uint8_t)((bcd -'a') + 10);
	else return 0xff;
	return bcd;
}

static inline uint16_t AsciiBcd2toByte(uint8_t bcdHi, uint8_t bcdLo){
	uint8_t hi = AsciiBcdToValue(bcdHi);
	uint8_t lo = AsciiBcdToValue(bcdLo);
	if ((hi > 0x0f)||(lo > 0x0f)) return 0xffff;
	return (uint16_t)((hi<< 4) | lo);
}

/*
 * Kopiuje ciag  znakow z 'from' do 'to' az do napotkania '\0'. Jesli 'withTerminator' to kopiuje tez '\0'.
 * @return	adres znaku bufora 'to' za ktorym zakonczylo sie kopiowanie. Jesli 'withTerminator', to za '\0'.
 */
char * my_strcpy (char * to, const char * from, bool withTerminator);

/*
 * Kopiuje 'amount'  bajtow z 'from' do 'to'.
 * @return	adres znaku bufora 'to' za ktorym zakonczylo sie kopiowanie.
 */
static inline uint8_t * my_memcpy (uint8_t * to, const uint8_t * from, uint32_t amount){
	while (amount--){
		*to++ = *from++;
	}
	return to;
}

inline bool isDigitAscii(char a){ return (a >= '0') && (a <= '9');	}

//----------------------------------------------------
#ifdef __cplusplus
}
#endif
//----------------------------------------------------


#endif /* MYUTILS_H_ */
