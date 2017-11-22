/*
 * Fifo.cpp
 *
 *  Created on: 24 sty 2014
 *      Author: lechu
 */

#include "Fifo.h"


// -----------------------------------------------------------------
/** ***************************************************************
 * @brief  Konstruktor bufora fifo
 * @param  bufor	wskaznik do zaalokowanej pamieci dla fizycznego bufora bajtow
 * @param  size		rozmiar przekazywanej pamieci
 */
Fifo::Fifo(uint8_t *bufor, uint32_t size){
	bufSize = size;
	this->bufor = bufor;
	in = out = 0;
}

// -----------------------------------------------------------------
/** ***************************************************************
 * @brief  Dodaje 1 bajt do bufora fifo
 * @param  bajt	Bajt do umieszczenia w buforze
 * @return true jesli operacja sie powiodla, albo false, jesli bufor jest pelny
 */
bool Fifo::put(uint8_t bajt){
	uint32_t tmpIn = in;
	uint32_t tmpOut = out;
	if ((tmpIn + 1) == tmpOut) return false;	//bufor pelny, bo ! Trzeba co najmniej 1 wolny bajt w buforze
	if ((tmpOut == 0)&&(tmpIn == (bufSize-1))) return false; // bufor pelny, indeksy na koncu i poczatku tablicy
	bufor[tmpIn++] = bajt;		// zapis pod indeks in (pierwsze wolne) i inkrementacja indeksu
	in = (tmpIn >= (bufSize))? 0 : tmpIn;	//korekcja indeksu w strukturze
	return true;
}

// -----------------------------------------------------------------
/** ***************************************************************
 * @brief  Pobiera 1 bajt z bufora.
 * @return Zwraca znak (bajt) lub BUFFER_EMPTY_FLAG_U16, gdy pusty bufor
 */
uint16_t Fifo::get(){
	uint32_t tmpOut = out;
	uint32_t tmpIn = in;
	if (tmpOut == tmpIn) return BUFFER_EMPTY_FLAG_U16; //bufor pusty, bo in = out! Zwracamy 0xff00
	uint8_t wynik = bufor[tmpOut++];
	out = (tmpOut >= bufSize)? 0 : tmpOut;
	return wynik;
}

/** ***************************************************************
 * @brief  Pobiera z bufora 2 znaki i interpretuje je jako liczby Hex w zapisie Ascii
 * @return Zwraca znak (bajt) lub 0xffff, gdy pusty bufor lub pobrane znaki nie sa liczbami Hex
 */
uint16_t Fifo::getByteAs2HexAscii(){
	uint16_t val1, val2;
	val1 = get();
	val2 = get();
	if ((val1 == BUFFER_EMPTY_FLAG_U16)||(val2 == BUFFER_EMPTY_FLAG_U16)) return 0xffff;
	//val1 = ;
	//if (val1 == 0xffff) return  BUFFER_EMPTY_FLAG_U32;
	return AsciiBcd2toByte(val1, val2);
}

/** ***************************************************************
 * @brief  Pobiera z bufora 2 znaki i interpretuje je jako liczby Hex w zapisie Ascii
 * @return Zwraca znak (bajt) lub 0xffffffff, gdy pusty bufor lub pobrane znaki nie sa liczbami Hex
 */
uint32_t Fifo::get2BytesAs4HexAscii(){
	uint16_t val1 = getByteAs2HexAscii();
	uint16_t val2 = getByteAs2HexAscii();
	if ((val1 == 0xffff)||(val2 == 0xffff)) return 0xffffffff;
	return (val1 << 8)| val2;
}


/** ***************************************************************
 * @brief  Pobiera 1 slowo 16-bitowe z bufora.
 * Czyli dwa kolejne bajty - pierwszy jako starszy bajt i drugi jako mlodszy
 * @return Zwraca slowo (16-bitowe) lub BUFFER_EMPTY_FLAG_U32, gdy pusty bufor
 */
uint32_t Fifo::getUInt16(){
	uint16_t tmp = get();
	if (tmp == BUFFER_EMPTY_FLAG_U16) return BUFFER_EMPTY_FLAG_U32;
	uint16_t tmp2 = get();
	if (tmp2 == BUFFER_EMPTY_FLAG_U16) return BUFFER_EMPTY_FLAG_U32;
	return (tmp << 8) | tmp2;
}

/** ***************************************************************
 * @brief  Pobiera 1 slowo 32-bitowe z bufora.
 * Czyli 4 kolejne bajty - pierwszy jako najstarszy bajt
 * @param  result	Wskaznik do rezultatu operacji
 * @return Zwraca slowo (32-bitowe). Poprawna operacja zwraca w *result true, niepoprawna false
 */
uint32_t Fifo::getUInt32(bool *result){
	uint32_t tmp = getUInt16();
	if (tmp == BUFFER_EMPTY_FLAG_U32){ *result = false; return 0;}
	uint32_t tmp2 = getUInt16();
	if (tmp2 == BUFFER_EMPTY_FLAG_U32){ *result = false; return 0;}
	return (tmp << 16) | tmp2;
}

/** ***************************************************************
 * @brief  Odklada bajt jako dwa znaki Ascii do bufora fifo.
 * Najpierw starsza polowka bajtu ('0'..'F'), potem mlodsza polowka bajtu
 * @param  inValue	Bajt do zapisania
 * @return True jesli operacja sie powiodla, lub false, gdy zabraklo miejsca w buforze
 */
bool Fifo::putByteAs2AsciiBCD(uint8_t inValue){
	bool retVal = true;
	uint8_t val = (inValue >> 4) & 0xff;
	if (val > 9) put(val + 'A' - 10);
	else put(val + '0');
	val = inValue & 0x0f;
	if (val > 9){ if (!put(val + 'A' - 10)) retVal = false; }
	else { if (!put(val + '0')) retVal = false; }
	return retVal;
}

/** ***************************************************************
 * @brief  Odklada slowo 2 bajtowe jako 4 znaki Hex Ascii do bufora fifo, poczawszy od nastarszego bajtu
 * @param  inValue	Wartosc do zapisania
 * @return True jesli operacja sie powiodla, lub false, gdy zabraklo miejsca w buforze
 */
bool Fifo::put2BytesAs4AsciiBCD(uint16_t inValue){
	putByteAs2AsciiBCD(inValue >> 8);
	return putByteAs2AsciiBCD(inValue);
}

/** ***************************************************************
 * @brief  Przepisuje wszystkie bajty ze zrodla do swojego bufora
 * @param  src	Obiekt Fifo z ktorego beda pobierane bajty. Nie pozostana one w buforze zrodlowym
 * @return True jesli operacja sie powiodla, lub false, gdy zabraklo miejsca w buforze lub bufor zrodlowy to nullptr
 */
bool Fifo::putAll(Fifo * src){
	if (src == nullptr) return false;
	bool result = true;
	while (src->isNotEmpty()){
		if (!put(src->get())) result = false;
	}
	return result;
}

/** ***************************************************************
 * @brief  Dodaje tekst zakonczony zerem do bufora. (bez konczacego zera)
 * @param  string	Tekst zakonczony \0
 * @return True jesli operacja sie powiodla, lub false, gdy zabraklo miejsca w buforze
 */
bool Fifo::putString(const char * string){
	bool result = true;
	while (*string != '\0'){
		if (!put(*string++)) result = false;
	}
	return result;
}

/** ***************************************************************
 * @brief  Dodaje tekst zakonczony zerem do bufora. (bez konczacego zera) z limitem bajtow
 * @param  string	Tekst zakonczony \0
 * @return True jesli operacja sie powiodla, lub false, gdy zabraklo miejsca w buforze albo gdy przesłano "limit" bajtow
 */
bool Fifo::putString(const char * string, uint32_t limit){
	uint8_t znak;
	for ( ; limit > 0; limit--){
		znak = *string++;
		if (znak == '\0') return true;	// string mniejszy niz limit
		if (!put(znak)) return false;	// fifo juz pelne
	}
	if (*string == '\0') return true;	// string ma dokladnie limit bajtow
	return false;						// string jest dluzszy niz limit wiecej sie nie da
}

/** ***************************************************************
 * @brief  odklada do bufora dwa bajty - starszy a potem mlodszy bajt slowa 16-bitowego
 * @param  value slowo do zapisania do bufora
 * @return True jesli operacja sie powiodla, lub false, gdy zabraklo miejsca w buforze
 */
bool Fifo::putUint16(uint16_t value){
	if (!put(value>>8)) return false;
	if (!put(value)) return false;
	return true;
}

/** ***************************************************************
 * @brief  odklada do bufora 4 bajty poczawszy od najstarszego ze slowa 32-bitowego
 * @param  value slowo do zapisania do bufora
 * @return True jesli operacja sie powiodla, lub false, gdy zabraklo miejsca w buforze
 */
bool Fifo::putUint32(uint32_t value){
	if (!putUint16(value>>16)) return false;
	if (!putUint16(value)) return false;
	return true;
}

/** ***************************************************************
 * @brief  Zwraca ilosc bajtow czekajacych w buforze.
 * @return Ilosc bajtow znajdujacych sie aktualnie w buforze.
 */
uint32_t Fifo::get_count(){
	uint32_t tmpOut = out;
	uint32_t tmpIn = in;
	if (tmpIn >= tmpOut) return (tmpIn - tmpOut);
	else return ((bufSize - tmpOut) + tmpIn);
}

/** ***************************************************************
 * @brief  Sprawdzenie stanu bufora - czy jest pelny
 * @return True jesli bufor jest pelny, lub false, gdy jeszcze mozna zapisac co najmniej 1 bajt
 */
bool Fifo::isFull(){
	uint32_t tmpOut = out;
	uint32_t tmpIn = in;
	if (++tmpIn == tmpOut) return true;//bufor pelny, bo ! Trzeba co najmniej 1 wolny bajt w buforze
	if ((tmpOut == 0)&&(tmpIn >= bufSize)) return true;// bufor pelny, indeksy na koncu i poczatku tablicy
	return false;
}

/** ***************************************************************
 * @brief  Zapisuje do docelowego bufora zawartosc wszystkich przechowywanych danych.
 * Dane zrodlowe nie sa tracone, poniewaz kazdy przepisywany bajt jest z powrotem dodawany do bufora
 * @param destination Bufor do ktorego odbedzie sie zapis
 * @return True operacja sie powiodla, lub false, gdy zabraklo miejsca w docelowym buforze
 */
bool Fifo::clone(Fifo * destination){
	bool result = true;
	//destination->flush();
	uint8_t znak;
	for (uint32_t dlug = get_count(); dlug > 0; dlug--){
		znak = get();
		put(znak);
		if (!destination->put(znak)) result = false;
	}
	return result;
}


