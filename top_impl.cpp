/*
 * mainmain.cpp
 *
 *  Created on: Mar 3, 2013
 *      Author: johnchronis
 */

#include "QueryIndex/QueryIndex.h"
#include "QueryIndex/QidsVault.h"
#include "QueryIndex/QueryInfo.h"
#include "QueryIndex/bitarea.h"
#include "QueryIndex/Query.h"
#include "QueryIndex/QueryIndex.h"
#include "QueryIndex/QidsVault.h"
#include "QueryIndex/QueryInfo.h"
#include "QueryIndex/bitarea.h"
#include "QueryIndex/Query.h"
#include <stdlib.h>
#include <list>
#include "TokenIndex/TokenDuplicateIndex.h"
#include "TokenIndex/TokenManager_Vector.h"
#include "TokenIndex/TokenStructs_Vector.h"
#include  "MapStuff/UnorderMap.h"
#include "Includes/core.h"
#include <bitset>
#include "JobScheduler/job_scheduler.h"
#include <pthread.h>

#include "JobScheduler/job_scheduler.h"
#include "JobScheduler/JobBarrier.h"
#include <stdlib.h>
#include <list>
#include "TokenIndex/TokenDuplicateIndex.h"
#include "TokenIndex/TokenManager_Vector.h"
#include "TokenIndex/TokenStructs_Vector.h"
#include  "MapStuff/UnorderMap.h"
#include "Includes/core.h"
#include "top_impl.h"
#include <bitset>
#include <vector>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "JobScheduler/JobBarrier.h"
#include "JobScheduler/JobDedup.h"
#include "JobScheduler/JobRun.h"
#include <sys/resource.h>
#include <stdlib.h>

#include "MatchStuff/Relations.h"
using namespace std;
class DocToken;
#define DOCSBUFFERSIZE 64

pthread_mutex_t mtx_jobmp;
pthread_mutex_t mtx_jobgr;
pthread_mutex_t mtx_jobbar;
vector<int> doctokens;
int startq = 0;
bool newfile = false;
bool runendrunned = true;
mple::QueryIndex * indexx;
mple::DuplicateIndex * exactmanager;
mple::DuplicateIndex * exactdocmanager;
mple::TokenManager * globalmanager;
mple::TokenManager * docmanager;
mple::bitarea * results;
mple::JobScheduler* js;
mple::JobScheduler* matchdocjs;
char** matchdocjs_buff;

std::vector<DocID> globaldocs;
std::vector<DocID> docids;

mple::Relations * rel[THREADS];

pthread_cond_t cond_barr;
pthread_mutex_t mtx_barr;
pthread_cond_t cond_barr11;
pthread_mutex_t mtx_barr11;
int count_barr;
mple::FlagIter batchiter[28][4];

int count_match_barr;
pthread_cond_t cond_match_barr;
pthread_mutex_t mtx_match_barr;

int batchno = 0;
std::vector<int> *wordbatchv;
bool gnflag = false;
bool match = false;
static int cou = 0;
char* cur_doc_str;
int v;


clock_t c_start;
clock_t c_end;
int w_start;


typedef struct {
    unsigned long size,resident,share,text,lib,data,dt;
} statm_t;

int GetRamInKB()
{
	statm_t result;
  const char* statm_path = "/proc/self/statm";

  FILE *f = fopen(statm_path,"r");
  if(!f){
    perror(statm_path);
    abort();
  }
  if(7 != fscanf(f,"%ld %ld %ld %ld %ld %ld %ld",
    &result.size,&result.resident,&result.share,&result.text,&result.lib,&result.data,&result.dt))
  {
    perror(statm_path);
    abort();
  }

  fclose(f);
  return (result.resident * getpagesize())/1024;
}



struct globaldocstuff {
    DocID did_;
    char* cur_doc_str_;
};



pthread_mutex_t mtx_empty_spot;
pthread_cond_t cond_empty_spot;
std::queue<int> empty_spot;

ErrorCode InitializeIndex() {

    for (int i = 0; i < THREADS; ++i)
        rel[i] = new mple::Relations();

    pthread_mutex_init(&mtx_jobmp, NULL);
    pthread_mutex_init(&mtx_jobgr, NULL);

    pthread_mutex_init(&mtx_empty_spot, NULL);
    pthread_cond_init(&cond_empty_spot, NULL);
    for (int i = 0; i < DOCSBUFFERSIZE; ++i) {
        empty_spot.push(i);
    }

    js = new mple::JobScheduler(THREADS);
    matchdocjs = new mple::JobScheduler(1);
    gnflag = false;

    matchdocjs_buff = new char*[DOCSBUFFERSIZE];
    for (int i = 0; i < DOCSBUFFERSIZE; i++) {
        matchdocjs_buff[i] = new char[MAX_DOC_LENGTH];
    }
    pthread_cond_init(&cond_barr, NULL);
    pthread_cond_init(&cond_match_barr, NULL);
    pthread_mutex_init(&mtx_barr, NULL);
    pthread_mutex_init(&mtx_match_barr, NULL);

    pthread_cond_init(&cond_barr11, NULL);
    pthread_mutex_init(&mtx_barr11, NULL);
    count_barr = 0;
    count_match_barr = 0;
    exactmanager = new mple::DuplicateIndex;
    exactdocmanager = new mple::DuplicateIndex;
    globalmanager = new mple::TokenManager(exactmanager);
    docmanager = new mple::TokenManager(exactdocmanager);
    indexx = new mple::QueryIndex(globalmanager);
    cur_doc_str = new char[MAX_DOC_LENGTH];
    wordbatchv = new vector<int> [MAX_WORD_LENGTH - MIN_WORD_LENGTH + 1];
    return EC_SUCCESS;

}

ErrorCode DestroyIndex() {
    c_end = clock();

    pthread_cond_destroy(&cond_barr);
    pthread_mutex_destroy(&mtx_barr);
    delete matchdocjs;
    delete js;

    for (int i = 0; i < DOCSBUFFERSIZE; i++) {
        delete[] matchdocjs_buff[i];
    }
    delete[] matchdocjs_buff;
    delete indexx;

    pthread_mutex_destroy(&mtx_empty_spot);
    pthread_cond_destroy(&cond_empty_spot);
    pthread_mutex_destroy(&mtx_jobmp);
    pthread_mutex_destroy(&mtx_jobgr);
    pthread_mutex_destroy(&mtx_jobbar);
    delete globalmanager;
    delete docmanager;
    delete exactmanager;
    delete exactdocmanager;
    delete[] wordbatchv;
    for (int i = 0; i < THREADS; ++i)
        delete rel[i];
    GetRamInKB();
    return EC_SUCCESS;


}

void RunEnd() {
    globalmanager->NewBatch();
    globaldocs.clear();

    for (int i = 0; i < 28; ++i) {
        for (std::vector<int>::iterator it = wordbatchv[i].begin();
                it != wordbatchv[i].end(); ++it) {
            mple::TokenId wid;
            wid.id_ = (*it) / 100;
            wid.length_ = (*it) % 100;
            globalmanager->GetTokenInfo(wid)->ClearIds();
        }
    }
    if (GetRamInKB() > 75 * 1024 * 1024)
	{

		globalmanager->Restart();
		for(int i=0;i<THREADS;++i)
		{
			delete rel[i];
			rel[i]=new mple::Relations;
		}

	}

    docids.clear();
    batchno++;
    for (int i = 0; i < MAX_WORD_LENGTH - MIN_WORD_LENGTH + 1; ++i)
        wordbatchv[i].clear();
    doctokens.clear();
    delete results;
}

ErrorCode StartQuery(QueryID query_id, const char* query_str,

MatchType match_type, unsigned int match_dist) {
    startq++;
    if (!match) {
        pthread_mutex_lock(&(js->mtx_batchEnd));
        while (js->batchEnd == 0) {
            pthread_cond_wait(&(js->cond_batchEnd), &(js->mtx_batchEnd));
        }
        js->batchEnd--;
        pthread_mutex_unlock(&(js->mtx_batchEnd));

        match = true;
    }
    mple::Qmid qmid;
    mple::Query Q(match_type, match_dist, query_id);
    char buff[32];
    int i = 0, j = 0;
    while (query_str[i] != '\0') {
        while (query_str[i] != '\0' && query_str[i] == ' ') {
            i++;
        }
        while (query_str[i] != '\0' && query_str[i] != ' ') {
            buff[j++] = query_str[i++];
        }
        buff[j] = '\0';
        j = 0;
        Q.AddToken(buff);
    }

    indexx->AddQuery(&Q, &qmid);
    return EC_SUCCESS;
}

ErrorCode EndQuery(QueryID query_id) {
    if (!match) {
        pthread_mutex_lock(&(js->mtx_batchEnd));
        while (js->batchEnd == 0) {
            pthread_cond_wait(&(js->cond_batchEnd), &(js->mtx_batchEnd));
        }
        js->batchEnd--;
        pthread_mutex_unlock(&(js->mtx_batchEnd));

        match = true;
    }

    indexx->RemoveQuery(query_id);
    return EC_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
ErrorCode MatchDocument(DocID doc_id, const char* doc_str) {
    cou++;
    static int our_id = 0;
    if (doc_id == 1 && startq > 100)
        newfile = true;

    if (match) {
        runendrunned = false;
        our_id = 0;
    }
    match = false;
    int spot;
    pthread_mutex_lock(&mtx_empty_spot);
    while (empty_spot.empty()) {
        pthread_cond_wait(&cond_empty_spot, &mtx_empty_spot);
    }
    spot = empty_spot.front();
    empty_spot.pop();
    js->docsofbatch++;
    pthread_mutex_unlock(&mtx_empty_spot);

    strcpy((matchdocjs_buff[spot]), doc_str);
    mple::JobDedup *JD = new mple::JobDedup(our_id, doc_id, spot);
    our_id++;

    matchdocjs->Schedule(JD);

    gnflag = true;

    docids.push_back(doc_id);

    return EC_SUCCESS;
}

static int jc = 0;
ErrorCode GetNextAvailRes(DocID* p_doc_id, unsigned int* p_num_res,
        QueryID** p_query_ids) {
    if (gnflag) {
        mple::JobRun *JR = new mple::JobRun();
        matchdocjs->Schedule(JR);
        gnflag = false;

    }
    pthread_mutex_lock(&(js->mtx_ResAvail));
    while (js->ResAvail == 0) {
        pthread_cond_wait(&(js->cond_ResAvail), &(js->mtx_ResAvail));
    }
    js->ResAvail--;
    pthread_mutex_unlock(&(js->mtx_ResAvail));
    *p_doc_id = 0;
    *p_num_res = 0;
    *p_query_ids = 0;
    Document *doc;
    pthread_mutex_lock(&(js->mtx_Res));    /////////////
    js->docsofbatch--;
    if (js->docsofbatch == 0) {
        runendrunned = true;
        RunEnd();
        pthread_mutex_lock(&(js->mtx_batchEnd));
        js->batchEnd++;
        pthread_cond_signal(&(js->cond_batchEnd));
        pthread_mutex_unlock(&(js->mtx_batchEnd));

    }
    if (js->docs.size() == 0) {
        pthread_mutex_unlock(&(js->mtx_Res));
        return EC_NO_AVAIL_RES;
    }
    doc = js->docs[0];
    js->docs.erase(js->docs.begin());
    pthread_mutex_unlock(&(js->mtx_Res));    ////////////


        *p_doc_id = docids[doc->doc_id - 1];

    *p_num_res = doc->num_res;
    *p_query_ids = doc->query_ids;



    return EC_SUCCESS;
}
