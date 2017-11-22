/*
 * HMI.cpp
 *
 *  Created on: 18 paź 2017
 *      Author: lechu
 */

#include "HMI.h"
//#include "main.h"

HMI hmi = HMI();


/** Wywolanie metody monitor() */
void static inline hmi_callback(){  hmi.monitor(); }



// periodycznie wykonywana funkcja monitor() opakowana w aku_callback()
QuickTask hmiQtsk(QuickTask::QT_PERIODIC, hmi_callback, HMI::TIME_PERIOD_MONITOR_MS);


HMI * HMI::getInstance(){ return &hmi; }
