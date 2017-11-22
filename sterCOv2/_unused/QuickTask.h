/*
 * QuickTask.h
 *
 *  Created on: 23 sty 2014
 *      Author: lechu
 */

#ifndef QUICKTASK_H_
#define QUICKTASK_H_

#include <stdint.h>

//const uint32_t QuickTask_INTERVAL_MILIS = 1;


//void runQuickTasks();

class QuickTask {
public:
	static constexpr uint32_t DEFAULT_TICK_INTERVAL_MILIS = 1;

	static uint32_t milisCounter;

	static void runQuickTasks();
	static void tick(uint32_t milis);

	typedef enum {
		QT_TIMER,	// tylko odmierza czas - trzeba sprawdzic przez is_stopped()
		QT_ONCE,		// po okreslonym czasie wykona QuickTask jednokrotnie.
		QT_PERIODIC	// raz puszczony puszcza sie sam co okreslony czas
	}QTType;

	typedef enum qtFlags{
		QT_DISABLED,				// nieaktywny
		QT_STOPPED,		// ukonczyl odliczanie
		QT_RUNNING,	// odlicza
	}QTFlags;

	volatile uint32_t timectr;
	uint32_t	updateValue;
	QuickTask *next;
	QTType	type;
	QTFlags	flags;
	void  (*qCallback)(void);
public:
	static uint32_t getCounter(){ return milisCounter; }

	QuickTask(QTType typ, void (*const task)(), const uint32_t updateMilis);

	/// Ustawia timemeter na okreslona ilosc milisekund
	/// Jesli nie istnieje, to dodaje go do kolejki timemeterow
	uint8_t is_stopped(){return (timectr == 0)? true: false;};
	void disable(){	flags = QuickTask::QT_DISABLED;	timectr = 0;}
	void set_milis(const uint32_t mili){
		timectr = mili;
		flags = QuickTask::QT_RUNNING;}
	void set_seconds(const uint8_t sek){set_milis(sek*1000); }
	void set_minutes(const uint8_t min){set_milis(min*1000*60); }
	void set_hours(const uint8_t godz){set_milis(godz*1000*60*60); }
	uint32_t get_milis(){return timectr;}
	void setIntervalMs(uint32_t updateMs){updateValue = updateMs;}
};

extern QuickTask* taskbase;

//----------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//----------------------------------------------------

void SysTick_Handler(void) __attribute__ ((interrupt));
//----------------------------------------------------
#ifdef __cplusplus
}
#endif
//----------------------------------------------------

#endif /* QUICKTASK_H_ */
