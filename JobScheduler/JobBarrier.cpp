/*
 * JobBarrier.cpp
 *
 *  Created on: Mar 29, 2013
 *      Author: johnchronis
 */

#include "JobBarrier.h"
namespace mple {

int JobBarrier::Run() {
	pthread_mutex_lock(mtx_wait);
	if((*count) > 0){
		(*count)-=1;
		pthread_cond_wait(cond_wait, mtx_wait);
	}else{
		pthread_cond_broadcast(cond_wait);
	}
	pthread_mutex_unlock(mtx_wait);

}
JobBarrier::~JobBarrier() {
	// TODO Auto-generated destructor stub
}

} /* namespace mple */
