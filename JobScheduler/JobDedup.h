/*
 * JobDedup.h
 *
 *  Created on: Mar 29, 2013
 *      Author: johnchronis
 */

#ifndef JOBDEDUP_H_
#define JOBDEDUP_H_
#include <stdlib.h>
#include "../QueryIndex/QueryIndex.h"
#include "../QueryIndex/QidsVault.h"
#include "../QueryIndex/QueryInfo.h"
#include "../QueryIndex/bitarea.h"
#include "../QueryIndex/Query.h"
//#include  "../MatchStuff/Relations.h"
#include "../JobScheduler/job_scheduler.h"
#include "job.h"


extern int totalhits;


namespace mple {

class JobDedup: public mple::Job {
public:
	int dn;
	int doc_id;
	int spot;
	int Run();
	JobDedup(int dn,DocID di, int spot){
		this->spot = spot;
		this->dn = dn;
		doc_id = di;
	}
	virtual ~JobDedup();
};

} /* namespace mple */
#endif /* JOBDEDUP_H_ */
