/*
 * Silnik.h
 *
 *  Created on: 22.08.2016
 *      Author: lechu
 */

//#include "stm32f4xx_hal_conf.h"

#ifndef SILNIK_H_
#define SILNIK_H_

class Silnik{
private:
	Gpio * gpio;
public:
	Silnik(Gpio * gpioPtr){
		gpio = gpioPtr;
	}

	void init(){
		gpio->setup(Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MediumSpeed);
	}

	void turnOn(){ gpio->pinSetUp(); }
	void turnOff(){ gpio->pinSetDown(); }
	void set(bool newState){ gpio->pinSet(newState); }
	bool isTurnedOn(){ return gpio->getOutputValue(); }

};



#endif /* SILNIK_H_ */
