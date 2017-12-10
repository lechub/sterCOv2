/*
 * Silnik.cpp
 *
 *  Created on: 10 gru 2017
 *      Author: lechu
 */

#include "Silnik.h"
#include "QuickTask.h"


/** Wywolanie metody monitor() */
void static inline silnik_callback(){ Silnik::poll(); }



// periodycznie wykonywana funkcja monitor() opakowana w aku_callback()
QuickTask silnikQtsk(QuickTask::QT_PERIODIC, silnik_callback, Silnik::TIME_PERIOD_POLL_MS);

Silnik * Silnik::firstSilnik = nullptr;

//HMI * HMI::getInstance(){ return &hmi; }



