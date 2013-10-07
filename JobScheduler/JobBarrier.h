/*
 * JobBarrier.h
 *
 *  Created on: Mar 29, 2013
 *      Author: johnchronis
 */

#ifndef JOBBARRIER_H_
#define JOBBARRIER_H_

#include "job.h"
#include <pthread.h>

namespace mple {

class JobBarrier: public mple::Job {
public:
	pthread_cond_t* cond_wait;
	pthread_mutex_t* mtx_wait;
	int* count;

	int Run();
	JobBarrier(pthread_cond_t* cw, pthread_mutex_t* mt, int *c) {
		cond_wait = cw;
		mtx_wait = mt;
		count = c;

	}
	virtual ~JobBarrier();
	
	bool isBarier() {
		return true;
	}
};

} /* namespace mple */
#endif /* JOBBARRIER_H_ */
