#ifndef JOB_SCHEDULER_H
#define	JOB_SCHEDULER_H
#define STOP_JOB NULL

#include <pthread.h>
#include <semaphore.h>
#include <iostream>

#include <google/dense_hash_map>
#include <google/sparsehash/libc_allocator_with_realloc.h>

#include <queue>
#include <map>
#include <vector>
#include <list>

#include "defn.h"
//#include "Queue.h"
#include "../top_impl.h"

using std::vector;
using std::list;

namespace mple {

class Job;
class JobPacket;

class JobScheduler {
public:
	std::vector<Document*> docs;

	pthread_mutex_t mtx_Res; //res queue

	pthread_mutex_t mtx_availableWork; //wakes/sleeps workers
	pthread_cond_t cond_availableWork;
	int availableWork;
	list<int> availableWeights;

	pthread_mutex_t mtx_batchEnd; //notifies start/end to start
	pthread_cond_t cond_batchEnd;
	int batchEnd;

	pthread_mutex_t mtx_ResAvail; //wakes GetNetxtAvail
	pthread_cond_t cond_ResAvail;
	int ResAvail;

	pthread_t *tids;

	int docsofbatch;

	JobScheduler(int num_of_threads = 1);
	virtual ~JobScheduler();

	// Schedule a job for execution.
	void Schedule(const vector<Job*>& jobs);
	void Schedule(Job* job);

	// Stop the scheduler.
	void Stop();

	int GetMyid(){
		return ids[pthread_self()];
	}

    void nojobleft(){
        pthread_mutex_lock(&mtx_availableWork);
	    while(availableWork >0){      
		    pthread_cond_wait(&cond_availableWork,&mtx_availableWork);
	    }	   
	    pthread_mutex_unlock(&mtx_availableWork);
    }

private:
	int num_of_threads_;
	std::queue<Job *> jobsQueue;
	google::dense_hash_map<pthread_t, int> ids;

	int activethreads;
	int next_id;

	static void *threadExecute(void *param);
	int fetchWork(vector<Job*>* jobs, int *id);

	DISALLOW_COPY_AND_ASSIGN(JobScheduler);
};

}  // namespace job_scheduler

#endif	// JOB_SCHEDULER_H
