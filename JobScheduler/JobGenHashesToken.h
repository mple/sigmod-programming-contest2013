/*
 * JobGenHashesToken.h
 *
 *  Created on: Apr 10, 2013
 *      Author: johnchronis
 */

#ifndef JOBGENHASHESTOKEN_H_
#define JOBGENHASHESTOKEN_H_

#include "job.h"

namespace mple {

class JobGenHashesToken: public mple::Job {
	int length;int sec;
public:
	JobGenHashesToken(int l,int s){length=l;sec=s;}
	virtual ~JobGenHashesToken();
	int Run();
	
	int getWeight() {
		if (length < 7 || length > 13) {
			return 10;
		}
		return 50;
	}
};

} /* namespace mple */
#endif /* JOBGENHASHESTOKEN_H_ */
