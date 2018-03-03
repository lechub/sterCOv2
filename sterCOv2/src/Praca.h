/*
 * TrybyPracy.h
 *
 *  Created on: 30.08.2016
 *      Author: lechu
 */

#ifndef INC_PRACA_H_
#define INC_PRACA_H_

//#include "Sterownik.h"
#include <stdint.h>

#include "Parameter.h"

class Praca{
public:

	static constexpr uint32_t TIME_PERIOD_PRACUJ_MS = 100;		// co 100 ms wywolanie pracuj()
	static constexpr uint32_t MODE_START_MS = 10*1000;		// 10 sekund od wlaczenia nic nie robi
	static constexpr uint32_t MODE_ROZPALANIE_MS = 10*1000;
	static constexpr uint32_t MODE_PODTRZYMANIE_MS = 10*1000;
	static constexpr uint32_t MODE_WYGASZANIE_MS = 10*1000;
	static constexpr uint32_t MODE_MANUAL_MS = 10*1000;

	typedef enum {
		START,
		ROZPALANIE,
		PODTRZYMANIE,
		WYGASZANIE,
		MANUAL,
		STOP
	}Mode;



private:
	Mode mode;
	uint32_t modeTimeMs;

	uint32_t timeDmuchawaMs;
	uint32_t timePodajnikMs;

	Parameter::SEZON sezon;

	inline bool timeTick(uint32_t timeSlice, uint32_t * timePtr){
		if (*timePtr > timeSlice){
			*timePtr -= timeSlice;
		}else{
			*timePtr = 0;
		}
		return true;
	}

public:
	Praca(
			//Sterownik * sterownik
			){
		//sterCO = sterownik;
		modeTimeMs = 0;
		timeDmuchawaMs = 0;
		timePodajnikMs = 0;
		mode = Mode::START;
		sezon = Parameter::SEZON::LATO;
	}

	const char * getModeDescr(){
		switch(mode){
		case Mode::START:	return "START";
		case Mode::ROZPALANIE:	return "ROZPALANIE";
		case Mode::PODTRZYMANIE:	return "PODTRZYMANIE";
		case Mode::WYGASZANIE:	return "WYGASZANIE";
		case Mode::MANUAL:	return "MANUAL";
		case Mode::STOP:	return "STOP";
		default:
			break;
		}
		return "BLAD!!!";
	}

	void setTrybLetni(Parameter::SEZON nowySezon){ sezon = nowySezon; }
	void setMode(Mode nowyTryb, uint32_t timeIntervalMs);
	void setManualMode(bool setManual){	mode = setManual ? Mode::MANUAL : Mode::START;	}
	bool isInManualMode(){ return mode == Mode::MANUAL; }

	inline Mode getMode(){ return mode; }

	void setDmuchawaTime(uint32_t timeIntervalMs){ timeDmuchawaMs = timeIntervalMs; }
	void setPodajnikTime(uint32_t timeIntervalMs){ timePodajnikMs = timeIntervalMs; }

	inline bool isModeTimeOut(){ return modeTimeMs == 0; }
	inline bool isDmuchawaTimeOut(){ return timeDmuchawaMs == 0; }
	inline bool isPodajnikTimeOut(){ return timePodajnikMs == 0; }

	inline bool timeTick(uint32_t timeSlice){
		timeTick(timeSlice, &modeTimeMs);
		timeTick(timeSlice, &timeDmuchawaMs);
		timeTick(timeSlice, &timePodajnikMs);
		return true;
	}

	inline void init(){
		setMode(Mode::START, MODE_START_MS);
	}

	void pracuj(){
		timeTick(TIME_PERIOD_PRACUJ_MS);
		if (mode == Mode::MANUAL){
			pracujManual();
		}else{
			pracujAutomat();
		}
	}

	void pracujManual();
	void pracujAutomat();


	void checkPompy();
	void checkAlarmy();

};

//extern Praca praca;

#endif /* INC_PRACA_H_ */
