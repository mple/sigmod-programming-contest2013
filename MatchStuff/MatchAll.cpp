/*
 * MatchAll1.cpp
 *
 *  Created on: Mar 23, 2013
 *      Author: vagos
 */

#include "../QueryIndex/QueryIndex.h"
#include "../QueryIndex/QidsVault.h"
#include "../QueryIndex/QueryInfo.h"
#include "../QueryIndex/bitarea.h"
#include "../QueryIndex/Query.h"
#include "../JobScheduler/JobMatchPrune.h"
#include "../JobScheduler/job_scheduler.h"
#include <stdlib.h>
#include <list>
#include "../TokenIndex/TokenDuplicateIndex.h"
#include "../TokenIndex/TokenManager_Vector.h"
#include "../TokenIndex/TokenStructs_Vector.h"
#include  "../MapStuff/UnorderMap.h"
#include "../Includes/core.h"
#include "../top_impl.h"
#include <bitset>
#include "../JobScheduler/JobBarrier.h"
#include "../JobScheduler/JobGenHashes.h"
#include "../JobScheduler/JobGenHashesToken.h"
#include <vector>
#include <unistd.h>
#include  "../MatchStuff/Relations.h"
using namespace std;

extern mple::JobScheduler* js;
extern pthread_cond_t cond_barr;
extern pthread_mutex_t mtx_barr;
extern mple::JobScheduler* js;
int count_genhashes;


namespace mple {


void SplitGenHashes() {
	count_genhashes = THREADS - 1;
	vector<Job*> jobs;


	for(int j=3; j>0; --j) {
		for (int i = 7; i < 11; ++i) {
			mple::JobGenHashes* JGH = new mple::JobGenHashes(i, j);
			jobs.push_back(JGH);


		}
	}
	for (int i = 7; i < 11; ++i) {
			mple::JobGenHashesToken* JGH2 = new mple::JobGenHashesToken(i, 0);
		jobs.push_back(JGH2);

		}




	for (int i = 27; i >= 0; --i) {
		if (i >= 7 && i <= 10)
			continue;
		for(int j=3; j>0; --j) {
			mple::JobGenHashes* JGH = new mple::JobGenHashes(i, j);
			jobs.push_back(JGH);


		}
	}
	for (int i = 27; i >= 0; --i) {
		if (i >= 7 && i <= 10)
			continue;
	mple::JobGenHashesToken* JGH2 = new mple::JobGenHashesToken(i, 0);
		jobs.push_back(JGH2);
	}


	for (int i = 0; i < THREADS; ++i) {
		JobBarrier* JB = new JobBarrier(&cond_barr, &mtx_barr, &count_genhashes);
		jobs.push_back(JB);
	}

	js->Schedule(jobs);
}
} //namespace mple
