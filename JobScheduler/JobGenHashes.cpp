/*
 * JobGenHashes.cpp
 *
 *  Created on: Apr 10, 2013
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
#include "../MatchStuff/Relations.h"
#include "JobGenHashes.h"
#include "../top_impl.h"
extern mple::TokenManager * globalmanager;
//extern mple::TokenManager * docmanager;
extern mple::bitarea * results;
extern std::vector<int> * wordbatchv;
extern mple::FlagIter batchiter[28][4];
//struct prehash{
//	size_t st;
//	lint
//};
//extern std::vector<>
extern mple::JobScheduler* js;
#include <list>
#include <string>

namespace mple {

JobGenHashes::~JobGenHashes() {
	// TODO Auto-generated destructor stub
}

int JobGenHashes::Run() {
		std::vector<ActiveToken> * dbrandnew = &globalmanager->indexes_[length].dbrandnew;
	for (int ii = 0;
			ii <dbrandnew->size(); ++ii) {

		TokenId tid;
		tid.id_ = (*dbrandnew)[ii].id_;
		tid.length_ = length + MIN_WORD_LENGTH;
		//cout<<"id "<<tid.id_<<" length "<<tid.length_<<endl;
		TokenInfo *ti = globalmanager->GetTokenInfo(tid);

		char* string = ti->GetTokenstring();
		int llength = length + MIN_WORD_LENGTH;
		//int count = 0;
		char res[llength];

		if (sec == 1) {
			globalmanager->indexes_[length].dhash_[0][mur()(string)].push_back(
					(*dbrandnew)[ii].id_); //gia to exact
			res[llength - 1] = '\0';
			for (unsigned int k = 0; k < llength; k++) {
				strncpy(res, string, k);
				strncpy(res + k, string + k + 1, llength - k - 1);
				globalmanager->indexes_[length].dhash_[1][mur()(res)].push_back(
						(*dbrandnew)[ii].id_);
				//	++count;
			}
		} else if (sec == 2) {
			res[llength - 2] = '\0';
			for (unsigned int j = 0; j < llength - 1; j++) {

				strncpy(res, string, j);
				for (unsigned int k = j + 1; k < llength; k++) {
					strncpy(res + j, string + j + 1, k - j - 1);
					strncpy(res + k - 1, string + k + 1, llength - k - 1);
					globalmanager->indexes_[length].dhash_[2][mur()(res)].push_back(
							(*dbrandnew)[ii].id_);
					//++count;
				}
			}
		} else {

			res[llength - 3] = '\0';
			for (unsigned int i = 0; i < llength - 2; i++) { //loop gia to -3
				strncpy(res, string, i);
				for (unsigned int j = i + 1; j < llength - 1; j++) {
					strncpy(res + i, string + i + 1, j - i - 1);
					for (unsigned int k = j + 1; k < llength; k++) {
						strncpy(res + j - 1, string + j + 1, k - j - 1);
						strncpy(res + k - 2, string + k + 1, llength - k - 1);
						globalmanager->indexes_[length].dhash_[3][mur()(res)].push_back(
								(*dbrandnew)[ii].id_);
						//					meion3[count] = mur()(res);
						//		++count;
					}
				}
			}
		}


		//	count = 0;
		//loop gia to -2

		//	count = 0;
		//loop gia to -1

	}
	int k;
	vector<Job*> jobs;
	if (sec == 3) {
		for(int j=0; j<4; ++j) {
			int i=length+j;
			if(i<0 || i>27) continue;
			int qsize3 =globalmanager->indexes_[i].qbrandnew3.size();

			//int dsize =globalmanager->indexes_[i].dbrandnew.size();
			int wpj = 1;

			for (k = 0; k < qsize3 - wpj; k += wpj) {
				mple::JobMatchPruneQD3* JMP = new mple::JobMatchPruneQD3(i+MIN_WORD_LENGTH, length+MIN_WORD_LENGTH,
				        k, k + wpj);
				jobs.push_back(JMP);
			}
			if (k != qsize3) {
				mple::JobMatchPruneQD3* JMP = new mple::JobMatchPruneQD3(i+MIN_WORD_LENGTH, length+MIN_WORD_LENGTH,
				        k, qsize3);
				jobs.push_back(JMP);
			}
		}
	} else if (sec == 2) {
		for(int j=-1; j<0; ++j) {
			int i=length+j;
			if(i<0 || i>27) continue;
			int qsize3 =globalmanager->indexes_[i].qbrandnew3.size();

			//int dsize =globalmanager->indexes_[i].dbrandnew.size();
			int wpj = 1;

			for (k = 0; k < qsize3 - wpj; k += wpj) {
				mple::JobMatchPruneQD3* JMP = new mple::JobMatchPruneQD3(i+MIN_WORD_LENGTH, length+MIN_WORD_LENGTH,
				        k, k + wpj);
				jobs.push_back(JMP);
			}
			if (k != qsize3) {
				mple::JobMatchPruneQD3* JMP = new mple::JobMatchPruneQD3(i+MIN_WORD_LENGTH, length+MIN_WORD_LENGTH,
				        k, qsize3);
				jobs.push_back(JMP);
			}
		}
		
		for(int j=0; j<3; ++j) {
			int i=length+j;
			if(i<0 || i>27) continue;
			int wpj = 1;
			int qsize2 =globalmanager->indexes_[i].qbrandnew2.size();

			for (k = 0; k < qsize2 - wpj; k += wpj) {
				mple::JobMatchPruneQD2* JMP = new mple::JobMatchPruneQD2(i+MIN_WORD_LENGTH, length+MIN_WORD_LENGTH,
				        k, k + wpj);
				jobs.push_back(JMP);
			}
			if (k != qsize2) {
				mple::JobMatchPruneQD2* JMP = new mple::JobMatchPruneQD2(i+MIN_WORD_LENGTH, length+MIN_WORD_LENGTH,
				        k, qsize2);
				jobs.push_back(JMP);
			}
		}


	} else if (sec==1) {
		for(int j=-3; j<-1; ++j) {
			int i=length+j;
			if(i<0 || i>27) continue;
			int qsize3 =globalmanager->indexes_[i].qbrandnew3.size();

			//int dsize =globalmanager->indexes_[i].dbrandnew.size();
			int wpj = 1;

			for (k = 0; k < qsize3 - wpj; k += wpj) {
				mple::JobMatchPruneQD3* JMP = new mple::JobMatchPruneQD3(i+MIN_WORD_LENGTH, length+MIN_WORD_LENGTH,
				        k, k + wpj);
				jobs.push_back(JMP);
			}
			if (k != qsize3) {
				mple::JobMatchPruneQD3* JMP = new mple::JobMatchPruneQD3(i+MIN_WORD_LENGTH, length+MIN_WORD_LENGTH,
				        k, qsize3);
				jobs.push_back(JMP);
			}
		}
		for(int j=-2; j<0; ++j) {
			int i=length+j;
			if(i<0 || i>27) continue;
			int wpj = 1;
			int qsize2 =globalmanager->indexes_[i].qbrandnew2.size();

			for (k = 0; k < qsize2 - wpj; k += wpj) {
				mple::JobMatchPruneQD2* JMP = new mple::JobMatchPruneQD2(i+MIN_WORD_LENGTH, length+MIN_WORD_LENGTH,
				        k, k + wpj);
				jobs.push_back(JMP);
			}
			if (k != qsize2) {
				mple::JobMatchPruneQD2* JMP = new mple::JobMatchPruneQD2(i+MIN_WORD_LENGTH, length+MIN_WORD_LENGTH,
				        k, qsize2);
				jobs.push_back(JMP);
			}
		}
		
		
		for(int j=-1; j<2; ++j) {
			int i=length+j;
			if(i<0 || i>27) continue;
			int wpj = 30;
			int qsize1 =globalmanager->indexes_[i].qbrandnew1.size();

			for (k = 0; k < qsize1 - wpj; k += wpj) {
				mple::JobMatchPruneQD1* JMP = new mple::JobMatchPruneQD1(i+MIN_WORD_LENGTH, length+MIN_WORD_LENGTH,
				        k, k + wpj);
				jobs.push_back(JMP);
			}
			if (k != qsize1) {
				mple::JobMatchPruneQD1* JMP = new mple::JobMatchPruneQD1(i+MIN_WORD_LENGTH, length+MIN_WORD_LENGTH,
				        k, qsize1);
				jobs.push_back(JMP);
			}
		}
		for(int j=0; j<1; ++j) {
			int i=length+j;
			if(i<0 || i>27) continue;
			int wpj = 30;
			int qsize0 =globalmanager->indexes_[i].qbrandnew0.size();
			for (k = 0; k < qsize0 - wpj; k += wpj) {
				mple::JobMatchPruneQD0* JMP = new mple::JobMatchPruneQD0(i+MIN_WORD_LENGTH, length+MIN_WORD_LENGTH,
						k, k + wpj);
				jobs.push_back(JMP);
			}
			if (k != qsize0) {
				mple::JobMatchPruneQD0* JMP = new mple::JobMatchPruneQD0(i+MIN_WORD_LENGTH, length+MIN_WORD_LENGTH,
						k, qsize0);
				jobs.push_back(JMP);
			}
		}

	}
	js->Schedule(jobs);
}
} /* namespace mple */
