/*
 * QuickTask.cpp
 *
 *  Created on: 23 sty 2014
 *      Author: lechu
 */

/*
 * QuickTask.c
 *
 *  Created on: 24-07-2013
 *      Author: lechu
 */

#include "QuickTask.h"

QuickTask * taskbase = 0;
uint32_t QuickTask::milisCounter;

void QuickTask::tick(uint32_t milis){
	QuickTask::milisCounter += milis;
	QuickTask * t = taskbase;
	while (t != 0){
		if (t->flags == QuickTask::QT_RUNNING){
			if (t->timectr > milis) t->timectr -= milis;
			else {
				t->flags = QuickTask::QT_STOPPED;
				t->timectr = 0;
			}
		}
		t = t->next;
	}
}

void QuickTask::runQuickTasks(){
	QuickTask * tsk = taskbase;
	while (tsk != nullptr){
		if (tsk->flags == QuickTask::QT_STOPPED){
			switch (tsk->type){
			case QuickTask::QT_PERIODIC:{
				tsk->timectr = tsk->updateValue;
				tsk->flags = QuickTask::QT_RUNNING;
				tsk->qCallback();// wykonanie zadania
				//t->task();
			}
				break;
			case QuickTask::QT_ONCE:{
				tsk->flags = QuickTask::QT_DISABLED;
				tsk->qCallback();// wykonanie zadania
				//t->task();
			}
				break;
			default:
				break;
			}
		}
		tsk = tsk->next;
	}
}
/*
void qt_configure(QuickTask* const qt, QTType typ, void (*const task)(), const uint32_t updateMilis){
	qt->flags = QT_DISABLED;
	qt->type = typ;
	qt->qCallback = task;
	qt->updateValue = updateMilis;
	if (taskbase == 0){	// jeszcze nie ma zadnych timemeterow, wiec ten jest pierwszy
		taskbase = qt;
		taskbase->next = 0;
		qt->flags = QT_RUNNING;
		return;
	}

	QuickTask * tmp = taskbase;	// kolejka timemeterow

	for(; tmp->next != 0; tmp = tmp->next) {
		if (tmp == qt) return;	// jest juz w kolejce timerow
	}

	if (tmp == qt) return;	// dodatkowy test dla ostatniego
	// Jednak ten timer nie jest na liscie - trzeba go dolozyc
	qt->next = 0;
	tmp->next = qt;
	qt->flags = QT_RUNNING;
}
 */
QuickTask::QuickTask(QTType typ, void (*const task)(), const uint32_t updateMilis){
	flags = QT_DISABLED;
	type = typ;
	qCallback = task;
	updateValue = updateMilis;
	if (taskbase == 0){	// jeszcze nie ma zadnych timemeterow, wiec ten jest pierwszy
		taskbase = this;
		taskbase->next = 0;
		flags = QT_RUNNING;
		return;
	}

	QuickTask * tmp = taskbase;	// kolejka timemeterow

	for(; tmp->next != 0; tmp = tmp->next) {
		if (tmp == this) return;	// jest juz w kolejce timerow
	}

	if (tmp == this) return;	// dodatkowy test dla ostatniego
	// Jednak ten timer nie jest na liscie - trzeba go dolozyc
	next = 0;
	tmp->next = this;
	flags = QT_RUNNING;
}



//-------------------------------------------------------
void SysTick_Handler(void){
	QuickTask::tick(QuickTask::DEFAULT_TICK_INTERVAL_MILIS);

}

//-------------------------------------------------------


