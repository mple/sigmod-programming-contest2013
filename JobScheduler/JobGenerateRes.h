/*
 * JobGenerateRes.h
 *
 *  Created on: Mar 26, 2013
 *      Author: johnchronis
 */

#ifndef JOBGENERATERES_H_
#define JOBGENERATERES_H_

#include "job.h"

namespace mple {

class JobGenerateRes: public mple::Job {
public:
	int document;
	int Run();
	JobGenerateRes(int d) {
		document = d;
	}
	virtual ~JobGenerateRes();
};

} /* namespace mple */
#endif /* JOBGENERATERES_H_ */
