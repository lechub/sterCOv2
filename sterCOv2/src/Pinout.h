/*
 * Pinout.h
 *
 *  Created on: 01.12.2016
 *      Author: lechu
 */

#ifndef PINOUT_H_
#define PINOUT_H_

#include "Parameter.h"
#include "Gpio.h"
#include "Led.h"

class Pinout {

public:
//	Gpio gpioPodajnik = Gpio(GPIOA,8, Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::Speed2);
//	Gpio gpioDmuchawa = Gpio(GPIOA,9, Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::Speed2);
//	Gpio gpioPompaCO = Gpio(GPIOA,10, Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::Speed2);
//	Gpio gpioPompaCWU = Gpio(GPIOA,11, Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::Speed2);
//
	Gpio gpioPodajnik = Gpio(GPIOA,8);
	Gpio gpioDmuchawa = Gpio(GPIOA,9);
	Gpio gpioPompaCO = Gpio(GPIOA,10);
	Gpio gpioPompaCWU = Gpio(GPIOA,11);

	Gpio gpioSCL = Gpio(GPIOB,6);
	Gpio gpioSDA = Gpio(GPIOB,7);
	Gpio gpioLcdReset = Gpio(GPIOC,7);


//	Gpio regulatorPokojowy = Gpio(GPIOC, 0, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);
//	Gpio termik = Gpio(GPIOA, 4, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);

//	Gpio gpioKey1 =  Gpio(GPIOC,6, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);
//	Gpio gpioKey2 =  Gpio(GPIOC,5, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);
//	Gpio gpioKey3 =  Gpio(GPIOC,8, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);
//	Gpio gpioKey4 =  Gpio(GPIOA,12, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);
//	Gpio gpioImpPush =  Gpio(GPIOC,9, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);
//	Gpio gpioImpLeft =  Gpio(GPIOB,8, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);
//	Gpio gpioImpRight =  Gpio(GPIOB,9, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);

	Gpio gpioKey1 =  Gpio(GPIOC,6);
	Gpio gpioKey2 =  Gpio(GPIOC,5);
	Gpio gpioKey3 =  Gpio(GPIOC,8);
	Gpio gpioKey4 =  Gpio(GPIOA,12);
	Gpio gpioImpPush =  Gpio(GPIOC,9);
	Gpio gpioImpLeft =  Gpio(GPIOB,8);
	Gpio gpioImpRight =  Gpio(GPIOB,9);
//	Gpio btnMenu = Gpio(GPIOA,1);
//	Gpio btnOK = Gpio(GPIOA,1);
//	Gpio btnUp = Gpio(GPIOA,1);
//	Gpio btnDown = Gpio(GPIOA,1);


//	Led ledAlarm	= Led(GPIOA,1, Led::Type::ACTIVELOW, Led::Mode::SWIECI );
//	Led ledWentylator	= Led(GPIOA,1, Led::Type::ACTIVELOW, Led::Mode::SWIECI);
//	Led ledPodajnik	= Led(GPIOA,1, Led::Type::ACTIVELOW, Led::Mode::SWIECI);
//	Led ledPompaCO	= Led(GPIOA,1, Led::Type::ACTIVELOW, Led::Mode::SWIECI);
//	Led ledPompaCWU	= Led(GPIOA,1, Led::Type::ACTIVELOW, Led::Mode::SWIECI);

	void setup(){
		gpioPodajnik.setup(Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::LowSpeed);
		gpioDmuchawa.setup(Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MaximumSpeed);
		gpioPompaCO.setup(Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::LowSpeed);
		gpioPompaCWU.setup(Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::LowSpeed);



		gpioKey1.setup(Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::HighSpeed);
		gpioKey2.setup(Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::HighSpeed);
		gpioKey3.setup(Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::HighSpeed);
		gpioKey4.setup(Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::HighSpeed);
		gpioImpPush.setup(Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::HighSpeed);
		gpioImpLeft.setup(Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::HighSpeed);
		gpioImpRight.setup(Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::HighSpeed);

	}

};

extern Pinout pins;

#endif /* PINOUT_H_ */
