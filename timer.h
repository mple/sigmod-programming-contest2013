/*
 * timer.h
 *
 *  Created on: Mar 29, 2013
 *      Author: johnchronis
 */

#ifndef TIMER_H_
#define TIMER_H_
#include <sys/time.h>
extern long long int a1,a2,a3,a4,a5,a6,a55,a66,a7,a8,a88,a99,a9;
extern long long time1,time2,time3,stime1,stime2,stime3,time4,stime4;
extern long long int first_for;
extern long long int second_for;
int MGetClockTimeInMilliSec();
long long int MGetClockTimeInNanoSec();
void MPrintTime(int milli_sec);
#endif /* TIMER_H_ */
