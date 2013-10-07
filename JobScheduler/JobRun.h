/*
 * JobRun.h
 *
 *  Created on: Mar 30, 2013
 *      Author: johnchronis
 */

#ifndef JOBRUN_H_
#define JOBRUN_H_

#include "../QueryIndex/bitarea.h"
#include "job.h"

namespace mple {

class JobRun: public mple::Job {
public:
	int Run();
	JobRun();
	virtual ~JobRun();
};

} /* namespace mple */
#endif /* JOBRUN_H_ */
