/*
 * Results.cpp
 *
 *  Created on: Mar 23, 2013
 *      Author: vagos
 */

#include <vector>
#include "../TokenIndex/TokenStructs_Vector.h"
//#include "TidToWid.h"
#include <unordered_map>
#include "../QueryIndex/QueryIndex.h"
#include "../QueryIndex/QidsVault.h"
#include "../QueryIndex/QueryInfo.h"
#include "../QueryIndex/bitarea.h"
#include "../QueryIndex/Query.h"
#include "../JobScheduler/JobGenerateRes.h"
#include "../JobScheduler/job_scheduler.h"
#include "../JobScheduler/JobBarrier.h"
#include <stdlib.h>
#include <list>
#include <unistd.h>
#include "../TokenIndex/TokenDuplicateIndex.h"
//#include "TokenIndex/TokenIndexBySize.h"
#include "../TokenIndex/TokenManager_Vector.h"
#include "../TokenIndex/TokenStructs_Vector.h"
#include  "../MapStuff/UnorderMap.h"
#include "../Includes/core.h"
#include "../top_impl.h"
#include "Relations.h"
extern mple::TokenManager * globalmanager;
extern mple::bitarea * results;
extern std::vector<DocID> globaldocs;
extern mple::JobScheduler* js;
extern pthread_cond_t cond_barr;
extern pthread_mutex_t mtx_barr;
extern int count_barr;
extern std::vector<int> * wordbatchv;
extern mple::Relations * rel[THREADS];
using namespace std;
namespace mple {


void GenerateRes(int start,int end,int length){

	//iterate on wids
	std::vector<wordsat> * curTids;
	//std::unordered_map<TokenId,char,tidhash> * curTids;
	//std::cout<<"The number of docwords = "<<wordbatch.size()<<std::endl;
//		for (std::unordered_map<int, bool>::iterator it = wordbatch.begin();
//				it != wordbatch.end(); ++it){
//	cout<<length<<" "<<start<<" "<<end<<endl;
		for(int iiii=start;iiii<end;iiii++)
		//for(std::vector<int>::iterator ittt=wordbatchv[sss].begin();ittt!=wordbatchv[sss].end();++ittt)
		{
			int ittt=wordbatchv[length][iiii];
		//iterate on tids
			for (int relt = 0 ; relt < THREADS ; ++relt){

				WidToTid * wttid = rel[relt]->GetTids((ittt));
				if (wttid == NULL)
					continue;
				curTids = wttid->GetVector();
				//curTids = wttid->GetMap();
				for (int i = 0 ; i<curTids->size() ; ++i){
				//for(auto &x : *curTids){
					TokenId tid = (*curTids)[i].tid;
					//TokenId tid = x.first;
					TokenInfo * qt = globalmanager->GetTokenInfo(tid);
					TokenId wid;
					wid.id_ = (ittt)/100;
					wid.length_ = (ittt)%100;
					TokenInfo * dt = globalmanager->GetTokenInfo(wid);
					//char tmp = 0; (FIX)
					char tmp = 127;
					results->SetStauros( (dt->GetDocidsPointer()),(qt->GetQmidsPointer()), (*curTids)[i].matchtype);
				}
			}
			//set res this tid all docs with this wids


		}
//		for (int i = 0 ; i< globaldocs.size(); ++i){
//					unsigned int Nresults;
//					QueryID * ResQueryIds;
//					results->results(indexx->GetVault(), &Nresults, &ResQueryIds,globaldocs[i]);
//					//results->printall(i);
//					Document doc;
//					doc.doc_id = globaldocs[i];
//					doc.num_res = Nresults;
//					doc.query_ids = 0;
//			//cout<<" nresults\t\t"<<Nresults<<endl;
//
//					if (Nresults)
//						doc.query_ids = ResQueryIds;
//
//					docs.push_back(doc);
//		}

}
	
class JobGenRes: public mple::Job {
	int length;
	int end;
	int start;
public:
	int Run()
	{

		GenerateRes(start,end,length);
	}
	JobGenRes(int s,int e,int l){start=s;end=e;length=l;}
	virtual ~JobGenRes(){}
};
int scount_barr;
void scheduleGenRes(int length) {
	int words_per_job, i;
	length -= MIN_WORD_LENGTH;
	int max = wordbatchv[length].size();

	vector<Job*> jobs;
	//words_per_job = ((max) / ((globalmanager->size(length) + 1))) + 1;
	words_per_job = (max
			/ (globalmanager->BatchQtokens(length + MIN_WORD_LENGTH) + 1));
	words_per_job /= 4;
	words_per_job++;
	//words_per_job*=24;
	//words_per_job = max;

	for (i = 0; i < max - words_per_job; i += words_per_job) {
		JobGenRes* JMP1 = new JobGenRes(i, i + words_per_job, length);
		jobs.push_back(JMP1);
	}
	if (i != max) {
		JobGenRes* JMP1 = new JobGenRes(i, max, length);
		jobs.push_back(JMP1);
	}
	js->Schedule(jobs);
}
void SplitGenerateRes() {
    js->nojobleft();
	count_barr = THREADS - 1;
	
	vector<Job*> jobs;
	for (int i = 0; i < THREADS; ++i) {
		JobBarrier* JB = new JobBarrier(&cond_barr, &mtx_barr, &count_barr);
		jobs.push_back(JB);
	}
	js->Schedule(jobs);

	
	for (int l = 8; l >= MIN_WORD_LENGTH; --l) {
		scheduleGenRes(l);
	}
	for (int l = 9; l <= MAX_WORD_LENGTH; ++l) {
		scheduleGenRes(l);
	}
	scount_barr = THREADS - 1;
	jobs.clear();
	for (int i = 0; i < THREADS; ++i) {
		JobBarrier* JB = new JobBarrier(&cond_barr, &mtx_barr, &scount_barr);
		jobs.push_back(JB);
	}
	js->Schedule(jobs);

	jobs.clear();
	for (int i = 0; i < globaldocs.size(); ++i) {
		JobGenerateRes *JGR1 = new JobGenerateRes(i);
		jobs.push_back(JGR1);
	}
	js->Schedule(jobs);
}
}
