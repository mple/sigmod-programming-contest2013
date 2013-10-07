#include "job_scheduler.h"
#include "job.h"
#include <iostream>

using namespace std;

namespace mple {

JobScheduler::JobScheduler(int num_of_threads) {
	if (num_of_threads < 1)
		num_of_threads = 1;

	activethreads = 0;
	docsofbatch=0;
	tids = new pthread_t[num_of_threads];
	this->num_of_threads_ = num_of_threads;

	availableWork = 0;
	batchEnd = 1;
	ResAvail = 0;

	ids.set_empty_key(-1);

	pthread_mutex_init(&mtx_availableWork, NULL);
	pthread_mutex_init(&mtx_batchEnd, NULL);
	pthread_mutex_init(&mtx_ResAvail, NULL);
	pthread_mutex_init(&mtx_Res, NULL);

	pthread_cond_init(&cond_ResAvail,NULL);
	pthread_cond_init(&cond_availableWork,NULL);
	pthread_cond_init(&cond_batchEnd,NULL);

	for (int i = 0; i < num_of_threads; ++i) {
		pthread_create(&tids[i], NULL, &JobScheduler::threadExecute,
				(void *) this);
		ids[tids[i]] = i;
	}
}

JobScheduler::~JobScheduler() {
	for (int i = 0; i < this->num_of_threads_; ++i) {
		pthread_mutex_lock(&mtx_availableWork);
		jobsQueue.push(STOP_JOB);
		availableWork++;
		pthread_cond_signal(&cond_availableWork);
		pthread_mutex_unlock(&mtx_availableWork);
	}

	for (int i = 0; i < this->num_of_threads_; ++i) {
		pthread_join(tids[i], NULL);
	}

	pthread_mutex_destroy(&mtx_availableWork);
	pthread_mutex_destroy(&mtx_batchEnd);
	pthread_mutex_destroy(&mtx_Res);

	pthread_cond_destroy(&cond_ResAvail);
	pthread_cond_destroy(&cond_availableWork);
	pthread_cond_destroy(&cond_batchEnd);

	delete[] tids;
}

void JobScheduler::Schedule(const vector<Job*>& jobs) {
	pthread_mutex_lock(&mtx_availableWork);
	if (jobs.size() > 0 && jobs[0]->isBarier()) {
		int aWeight = 0;
		availableWeights.push_front(aWeight);
	}
	for (int i = 0; i < jobs.size(); ++i) {
		Job* job = jobs[i];
		jobsQueue.push(job); //add new job to queue
		availableWork++;
		if (availableWeights.size() == 0) {
			int aWeight = 0;
			availableWeights.push_front(aWeight);
		}
		availableWeights.back() += job->getWeight();
	}
	pthread_cond_signal(&cond_availableWork);
	pthread_mutex_unlock(&mtx_availableWork);
}

void JobScheduler::Schedule(Job* job) {
	pthread_mutex_lock(&mtx_availableWork);
	jobsQueue.push(job); //add new job to queue
	availableWork++;
	if (availableWeights.size() == 0) {
		int aWeight = 0;
		availableWeights.push_front(aWeight);
	}
	availableWeights.back() += job->getWeight();
	pthread_cond_signal(&cond_availableWork);
	pthread_mutex_unlock(&mtx_availableWork);
}

int JobScheduler::fetchWork(vector<Job*>* jobs, int * id) {
	pthread_mutex_lock(&mtx_availableWork);
	while(availableWork == 0){
        pthread_cond_signal(&cond_availableWork);
		pthread_cond_wait(&cond_availableWork,&mtx_availableWork);
	}
//	printf(" -- %d -- %d\n", availableWeights.size(), availableWeights.front());
	int maxWeight = 1;
	if (availableWeights.size() > 0)
		maxWeight = availableWeights.front() / (128*THREADS);
	if (maxWeight < 1) {
		maxWeight = 1;
	}
	while (availableWork > 0 && maxWeight > 0) {
		--availableWork;
		Job* next = jobsQueue.front();
		if (next != NULL) {
			maxWeight -= next->getWeight();
			if (availableWeights.size() > 0){
				availableWeights.front() -= next->getWeight();
				if (availableWeights.front() <= 0) {
					availableWeights.pop_front();
				}
			}
		}
		jobsQueue.pop();
		jobs->push_back(next);
		if (next == NULL || next->isBarier()) {
			break;
		}
	}
	pthread_mutex_unlock(&mtx_availableWork);
	return jobs->size();
}

void *JobScheduler::threadExecute(void *param) {
	vector<Job*> jobs;
	int id;
	int end = false;
	while (((JobScheduler *) param)->fetchWork(&jobs, &id) > 0) {
		for (int i = 0; i < jobs.size(); ++i) {
			if (jobs[i] == NULL) {
				end = true;
				break;
			}
			jobs[i]->Run();
			delete jobs[i];
		}
		jobs.clear();
		if (end) {
			break;
		}
	}
	pthread_exit(NULL);
}

} ; // namespace job_scheduler
