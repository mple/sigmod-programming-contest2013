/*
 * JobGenerateRes.cpp
 *
 *  Created on: Mar 26, 2013
 *      Author: johnchronis
 */

#include "../QueryIndex/QueryIndex.h"
#include "../QueryIndex/QidsVault.h"
#include "../QueryIndex/QueryInfo.h"
#include "../QueryIndex/bitarea.h"
#include "../QueryIndex/Query.h"
#include <stdlib.h>
#include <list>
#include "../TokenIndex/TokenDuplicateIndex.h"
#include "../TokenIndex/TokenManager_Vector.h"
#include "../TokenIndex/TokenStructs_Vector.h"
#include  "../MapStuff/UnorderMap.h"
#include "../Includes/core.h"
#include "../top_impl.h"
#include <bitset>
#include "../JobScheduler/job_scheduler.h"
#include <pthread.h>
#include "JobMatchPrune.h"
using namespace std;
extern mple::QueryIndex * indexx;
extern mple::bitarea * results;
extern std::vector<DocID> globaldocs;
extern mple::JobScheduler* js;
#include "JobGenerateRes.h"

namespace mple {

JobGenerateRes::~JobGenerateRes() {
	// TODO Auto-generated destructor stub
}
int JobGenerateRes::Run() {

	unsigned int Nresults;
	QueryID * ResQueryIds;
	results->results(indexx->GetVault(), &Nresults, &ResQueryIds,
			globaldocs[document]);
	Document *doc = new Document;
	doc->doc_id = globaldocs[document];
	doc->num_res = Nresults;
	doc->query_ids = 0;

	if (Nresults)
		doc->query_ids = ResQueryIds;
	pthread_mutex_lock(&(js->mtx_Res)); /////////////
	js->docs.push_back(doc);
	pthread_mutex_unlock(&(js->mtx_Res)); ////////////

	pthread_mutex_lock(&(js->mtx_ResAvail));
	js->ResAvail++;
	pthread_cond_signal(&(js->cond_ResAvail));
	pthread_mutex_unlock(&(js->mtx_ResAvail));
	return 0;
}

} /* namespace mple */
