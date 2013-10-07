#ifndef JOB_H
#define	JOB_H

#include "defn.h"

namespace mple
{

class Job
{
public:
	Job() { }
	virtual ~Job() { }
	// This method should be implemented by subclasses.
	virtual int Run() = 0;

	virtual bool isBarier() {
		return false;
	}

	virtual int getWeight() {
		return 1;
	}

private:
	DISALLOW_COPY_AND_ASSIGN(Job);
};


}  // namespace job_scheduler

#endif	// JOB_H
