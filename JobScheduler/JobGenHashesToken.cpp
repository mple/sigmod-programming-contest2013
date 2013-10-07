/*
 * JobGenHashesToken.cpp
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
#include "../MatchStuff/Relations.h"
#include "../top_impl.h"
#include "JobGenHashesToken.h"
#include "../JobScheduler/JobMatchPrune.h"
extern mple::TokenManager * globalmanager;
//extern mple::TokenManager * docmanager;
extern mple::bitarea * results;
extern int batchno;
extern std::vector<int> * wordbatchv;
extern mple::FlagIter batchiter[28][4];
extern mple::JobScheduler* js;
#include <list>
#include <string>
namespace mple {

JobGenHashesToken::~JobGenHashesToken() {
	// TODO Auto-generated destructor stub
}

int JobGenHashesToken::Run() {
	for (auto ii = globalmanager->indexes_[length].qbrandnew.begin();
			ii != globalmanager->indexes_[length].qbrandnew.end(); ii++) {

		TokenId tid;
		tid.id_ = ii->id_;
		tid.length_ = length + MIN_WORD_LENGTH;
		//cout<<"id "<<tid.id_<<" length "<<tid.length_<<endl;
		TokenInfo *ti = globalmanager->GetTokenInfo(tid);

		char* string = ti->GetTokenstring();
		int llength = length + MIN_WORD_LENGTH;
		//int count = 0;
		char res[llength];
		//if (sec == 3) {
		res[llength - 3] = '\0';
		for (unsigned int i = 0; i < llength - 2; i++) { //loop gia to -3
			strncpy(res, string, i);
			for (unsigned int j = i + 1; j < llength - 1; j++) {
				strncpy(res + i, string + i + 1, j - i - 1);
				for (unsigned int k = j + 1; k < llength; k++) {
					strncpy(res + j - 1, string + j + 1, k - j - 1);
					strncpy(res + k - 2, string + k + 1, llength - k - 1);
					globalmanager->indexes_[length].qhash_[3][mur()(res)].push_back(
							ii->id_);
					//					meion3[count] = mur()(res);
					//		++count;
				}
			}
		}
		//	count = 0;
		//loop gia to -2
		//} else if (sec == 2) {
		res[llength - 2] = '\0';
		for (unsigned int j = 0; j < llength - 1; j++) {

			strncpy(res, string, j);
			for (unsigned int k = j + 1; k < llength; k++) {
				strncpy(res + j, string + j + 1, k - j - 1);
				strncpy(res + k - 1, string + k + 1, llength - k - 1);
				globalmanager->indexes_[length].qhash_[2][mur()(res)].push_back(
						ii->id_);
				//++count;
			}
		}
		//	count = 0;
		//loop gia to -1
		//} else {
		res[llength - 1] = '\0';
		for (unsigned int k = 0; k < llength; k++) {
			strncpy(res, string, k);
			strncpy(res + k, string + k + 1, llength - k - 1);
			globalmanager->indexes_[length].qhash_[1][mur()(res)].push_back(
					ii->id_);
			//	++count;
		}

		globalmanager->indexes_[length].qhash_[0][mur()(string)].push_back(
				ii->id_); //gia to exact

		//}
	}
	if (batchno != 0) {
		vector<Job*> jobs;
		for (int j = -3; j < 4; ++j) {
			int i = length + j;
			if (i < 0 || i > 27)
				continue;
			int dsize = globalmanager->indexes_[i].dbrandnew.size();
			int wpj = 1;
			int k;
			wpj = 1;
			for (k = 0; k < dsize - wpj; k += wpj) {
				mple::JobMatchPrune1* JMP1 = new mple::JobMatchPrune1(
						i + MIN_WORD_LENGTH, length + MIN_WORD_LENGTH, k,
						k + wpj);
				jobs.push_back(JMP1);
			}
			if (k != dsize) {
				mple::JobMatchPrune1* JMP1 = new mple::JobMatchPrune1(
						i + MIN_WORD_LENGTH, length + MIN_WORD_LENGTH, k,
						dsize);
				jobs.push_back(JMP1);
			}
		}
		js->Schedule(jobs);
	}
}

} /* namespace mple */
