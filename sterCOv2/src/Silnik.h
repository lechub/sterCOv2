/*
 * Silnik.h
 *
 *  Created on: 22.08.2016
 *      Author: lechu
 */

//#include "stm32f4xx_hal_conf.h"

#ifndef SILNIK_H_
#define SILNIK_H_

#include <stdint.h>
#include "Gpio.h"

class Silnik{
public:
	static constexpr uint32_t TIME_PERIOD_POLL_MS = 20;			// co 10 milisekund poll()
	static constexpr uint8_t MAX_POWER = 10;			// 20ms/okres * 10 = 200ms/10 okresow
	static constexpr uint32_t TIME_CYCLE_MS = TIME_PERIOD_POLL_MS * MAX_POWER;	// 20ms/okres * 10 = 200ms/10 okresow
private:
	static Silnik * firstSilnik;
	Gpio * gpio;
	Silnik * nextSilnik;
	uint8_t wypelnienie = 10;
	bool turnedOn = false;

public:
	Silnik(Gpio * gpioPtr){
		gpio = gpioPtr;
		nextSilnik = firstSilnik;
		firstSilnik = this;	// dodanie silnika do kolejki - na koncu gdy wszystko zainicjowane
	}

	void init(){
		gpio->setup(Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MediumSpeed);
	}

	/**
	 * wlacza gpio dla silnika
	 */
	void switchOn(){ gpio->pinSetUp(); }
	/**
	 * wylacza gpio dla silnika
	 */
	void switchOff(){ gpio->pinSetDown(); }
	/**
	 * wlacza/wylacza gpio dla silnika
	 */
	void switchLevel(bool level){ if (level) switchOn(); else switchOff(); }
	/**
	 * zwraca aktualny stan gpio dla silnika
	 */
	bool isSwitchedOn() const { return gpio->getOutputValue(); }

	/**
	 * wlacza silnik z aktualnym wypelnieniem
	 */
	void turnOn(){ turnedOn = true; }
	/**
	 * wylacza silnik z aktualnym wypelnieniem
	 */
	void turnOff(){ turnedOn = false; }
	/**
	 * wlacza silnik z aktualnym wypelnieniem
	 */
	void set(bool newState){ if (newState) turnOn(); else turnOff(); }

	/**
	 * Zwraca aktualny stan silnika w zwiazku z wypelnieniem
	 */
	bool isTurnedOn() const { return turnedOn; }

	/**
	 * Ustawia wypelnienie (moc) dla silnika w zakresie od 0 do 10 (MAX_POWER)
	 */
	void setPower(uint8_t power){ wypelnienie = (power > MAX_POWER) ? MAX_POWER : power; }

	static void poll(){
		static uint8_t krok = 0;
		krok++;
		if (krok > 10) krok = 0;
		Silnik * sil = firstSilnik;
		while(sil != nullptr){
			if (sil->turnedOn){
			sil->switchLevel(sil->wypelnienie >= krok);
			sil = sil->nextSilnik;
			}else{
				sil->switchOff();
			}
		}
	}


};



#endif /* SILNIK_H_ */
