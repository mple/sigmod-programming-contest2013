/*
 * JobGenHashes.h
 *
 *  Created on: Apr 10, 2013
 *      Author: johnchronis
 */

#ifndef JOBGENHASHES_H_
#define JOBGENHASHES_H_

#include "job.h"

namespace mple {

class JobGenHashes: public mple::Job {
	int length;int sec;
public:
	JobGenHashes(int l,int s){length=l;sec=s;}
	virtual ~JobGenHashes();
	int Run();
	
	int getWeight() {
		if (length < 7 || length > 13) {
			return 5;
		}
		return  30;
	}
};

} /* namespace mple */
#endif /* JOBGENHASHES_H_ */
