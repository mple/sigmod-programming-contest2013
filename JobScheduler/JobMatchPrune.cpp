/*
 * JobMatchPrune.cpp
 *
 *  Created on: Mar 27, 2013
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
using namespace std;
pthread_mutex_t mv;
int couffnt = 0;
extern mple::TokenManager * globalmanager;
extern mple::Relations *rel[THREADS];
extern mple::JobScheduler* js;
#include <list>
extern int batchno;
namespace mple {


bool SetDimis(google::dense_hash_set<lint> &memory,
              Hash::iterator resultdocwids, TokenId tid, TokenInfo * qtoken,
              int docspace, int queryspace, bool forceset, int threadid, int diafora =
                  0) {
	for (std::vector<lint>::iterator id = resultdocwids->second.begin();
	        id != resultdocwids->second.end(); ++id) {

		TokenId docid;
		docid.id_ = (*id);
		docid.length_ = docspace;
		if (memory.find(docid.key()) != memory.end())
			continue;
		char result;
		forceset = false;
//		if(!strcmp("ymyri",globalmanager->GetTokenInfo(tid)->GetTokenstring()) && (queryspace==docspace))
//		{
//			cout<<"ymyri "<< globalmanager->GetTokenInfo(docid)->GetTokenstring()<<endl;
//		}
		//if(qtoken->GetRequests()==0) break;
		//cout<<"paei na sygkrinei "<<globalmanager->GetTokenInfo(docid)->GetTokenstring()<<qtoken->GetTokenstring()<<" me lengths "<< docspace<< " "<<queryspace<<endl;
		if (!forceset) {
			if ((result = stavroshtein(
			                  globalmanager->GetTokenInfo(docid)->GetTokenstring(),
			                  qtoken->GetTokenstring(), docspace, queryspace)) != 0) {
				//results->SetStauros(globalmanager->GetTokenInfo(docid)->GetDocidsPointer(),qtoken->GetQmidsPointer(),result);
				rel[threadid]->Add(docid, tid, result);
				memory.insert(docid.key());
			}

		} else {
			result = (diafora != 0) ? 0b00001000 << diafora : 0b01111111;
			//results->SetStauros(globalmanager->GetTokenInfo(docid)->GetDocidsPointer(),qtoken->GetQmidsPointer(),result);
			rel[threadid]->Add(docid, tid, result);
			memory.insert(docid.key());
		}
	}
}
void SetDimisAnapodi(google::dense_hash_set<lint> &memory,
                     Hash::iterator resultqtids, TokenId wid, TokenInfo * wtoken,
                     int docspace, int queryspace, bool forceset, int threadid, int diafora =
                         0) {
	for (std::vector<lint>::iterator id = resultqtids->second.begin();
	        id != resultqtids->second.end(); ++id) {

		TokenId qtid;
		qtid.id_ = (*id);
		qtid.length_ = queryspace;
		if (memory.find(qtid.key()) != memory.end())
			continue;
		char result;
		forceset = false;
		//if(globalmanager->GetTokenInfo(qtid)->GetRequests()==0 ) continue;
		//cout<<"paei na sygkrinei "<<wtoken->GetTokenstring()<<globalmanager->GetTokenInfo(qtid)->GetTokenstring()<<" me lengths "<< docspace<< " "<<queryspace<<endl;
		if (!forceset) {
			if ((result = stavroshtein(wtoken->GetTokenstring(),
			                           globalmanager->GetTokenInfo(qtid)->GetTokenstring(),
			                           docspace, queryspace)) != 0) {
				//results->SetStauros(wtoken->GetDocidsPointer(),globalmanager->GetTokenInfo(qtid)->GetQmidsPointer(),result);
				rel[threadid]->Add(wid, qtid, result);
				memory.insert(qtid.key());
			}
		} else {
			//results->SetStauros(wtoken->GetDocidsPointer(),globalmanager->GetTokenInfo(qtid)->GetQmidsPointer(),(diafora!=0)?0b00001000 << diafora : 0b01111111);
			result = (diafora != 0) ? 0b00001000 << diafora : 0b01111111;
			rel[threadid]->Add(wid, qtid, result);
			memory.insert(qtid.key());
		}

	}
}
void OnTheFly(char * string ,int ana,Hash* hash,google::dense_hash_set<lint> &memory,TokenId tid,TokenInfo * token,int  docspace,int queryspace,int threadid);
void QD3(int queryspace, int docspace, int threadid, int start3, int end3) {
	int diafora = abs(docspace - queryspace);
	/*Active QueryToken Lists*/
	std::vector<mple::ActiveToken> *lbrandnew3 =
	&globalmanager->indexes_[queryspace - MIN_WORD_LENGTH].qbrandnew3;
	std::vector<mple::ActiveToken> *lbrandnew2 =
	&globalmanager->indexes_[queryspace - MIN_WORD_LENGTH].qbrandnew2;
	std::vector<mple::ActiveToken> *lbrandnew1 =
	&globalmanager->indexes_[queryspace - MIN_WORD_LENGTH].qbrandnew1;
	std::vector<mple::ActiveToken> *lbrandnew0 =
	&globalmanager->indexes_[queryspace - MIN_WORD_LENGTH].qbrandnew0;
	/************/

	/* DocToken Hashes 0,-1,-2,-3 */
	google::dense_hash_map<size_t, vector<lint>> *hash0 =
	            &globalmanager->indexes_[docspace - MIN_WORD_LENGTH].dhash_[0];
	google::dense_hash_map<size_t, vector<lint>> *hash1 =
	            &globalmanager->indexes_[docspace - MIN_WORD_LENGTH].dhash_[1];
	google::dense_hash_map<size_t, vector<lint>> *hash2 =
	            &globalmanager->indexes_[docspace - MIN_WORD_LENGTH].dhash_[2];
	google::dense_hash_map<size_t, vector<lint>> *hash3 =
	            &globalmanager->indexes_[docspace - MIN_WORD_LENGTH].dhash_[3];
	/*****************************/
	if (docspace >= queryspace) {
		//edit3
		for (int johni = start3; johni < end3; johni++) {
//		for(vector<ActiveToken>::iterator it=lbrandnew->begin(); it!=lbrandnew->end(); ++it) {
			ActiveToken* it = &(*lbrandnew3)[johni];
			google::dense_hash_set<lint> memory;
			memory.set_empty_key(0);
			if (it->token_ == NULL)
				continue;
			if (diafora == 0) {
				OnTheFly(it->token_->GetTokenstring(),3,hash3,memory,TokenId(queryspace, it->id_),it->token_,docspace,queryspace,threadid);
			} else if (diafora == 1) {
				OnTheFly(it->token_->GetTokenstring(),2,hash3,memory,TokenId(queryspace, it->id_),it->token_,docspace,queryspace,threadid);
			} else if (diafora == 2) {
				OnTheFly(it->token_->GetTokenstring(),1,hash3,memory,TokenId(queryspace, it->id_),it->token_,docspace,queryspace,threadid);
			} else if (diafora == 3) {
				OnTheFly(it->token_->GetTokenstring(),0,hash3,memory,TokenId(queryspace, it->id_),it->token_,docspace,queryspace,threadid);				
			}
		}
		
	} else {
		for (int johni = start3; johni < end3; johni++) {
			//		for(vector<ActiveToken>::iterator it=lbrandnew->begin(); it!=lbrandnew->end(); ++it) {
			ActiveToken* it = &(*lbrandnew3)[johni];
			google::dense_hash_set<lint> memory;
			memory.set_empty_key(0);
			if (it->token_ == NULL)
				continue;
			if (diafora == 1) {
				OnTheFly(it->token_->GetTokenstring(),3,hash2,memory,TokenId(queryspace, it->id_),it->token_,docspace,queryspace,threadid);								
			} else if (diafora == 2) {
				OnTheFly(it->token_->GetTokenstring(),3,hash1,memory,TokenId(queryspace, it->id_),it->token_,docspace,queryspace,threadid);				
			} else if (diafora == 3) {
				OnTheFly(it->token_->GetTokenstring(),3,hash0,memory,TokenId(queryspace, it->id_),it->token_,docspace,queryspace,threadid);				
			}
		}
	}
}

void QD2(int queryspace, int docspace, int threadid,int start2,int end2) {
	int diafora = abs(docspace - queryspace);
	/*Active QueryToken Lists*/
	std::vector<mple::ActiveToken> *lbrandnew3 =
	&globalmanager->indexes_[queryspace - MIN_WORD_LENGTH].qbrandnew3;
	std::vector<mple::ActiveToken> *lbrandnew2 =
	&globalmanager->indexes_[queryspace - MIN_WORD_LENGTH].qbrandnew2;
	std::vector<mple::ActiveToken> *lbrandnew1 =
	&globalmanager->indexes_[queryspace - MIN_WORD_LENGTH].qbrandnew1;
	std::vector<mple::ActiveToken> *lbrandnew0 =
	&globalmanager->indexes_[queryspace - MIN_WORD_LENGTH].qbrandnew0;
	/************/

	/* DocToken Hashes 0,-1,-2,-3 */
	google::dense_hash_map<size_t, vector<lint>> *hash0 =
	            &globalmanager->indexes_[docspace - MIN_WORD_LENGTH].dhash_[0];
	google::dense_hash_map<size_t, vector<lint>> *hash1 =
	            &globalmanager->indexes_[docspace - MIN_WORD_LENGTH].dhash_[1];
	google::dense_hash_map<size_t, vector<lint>> *hash2 =
	            &globalmanager->indexes_[docspace - MIN_WORD_LENGTH].dhash_[2];
	google::dense_hash_map<size_t, vector<lint>> *hash3 =
	            &globalmanager->indexes_[docspace - MIN_WORD_LENGTH].dhash_[3];
	/*****************************/
	if (docspace >= queryspace) {
		//edit2
		for (int johni = start2; johni < end2; johni++) {
//		for(vector<ActiveToken>::iterator it=lbrandnew->begin(); it!=lbrandnew->end(); ++it) {
			ActiveToken* it = &(*lbrandnew2)[johni];
			google::dense_hash_set<lint> memory;
			memory.set_empty_key(0);
			if (it->token_ == NULL)
				continue;
			if (diafora == 0) {
				OnTheFly(it->token_->GetTokenstring(),2,hash2,memory,TokenId(queryspace, it->id_),it->token_,docspace,queryspace,threadid);
			} else if (diafora == 1) {
				OnTheFly(it->token_->GetTokenstring(),1,hash2,memory,TokenId(queryspace, it->id_),it->token_,docspace,queryspace,threadid);
			} else if (diafora == 2) {
				OnTheFly(it->token_->GetTokenstring(),0,hash2,memory,TokenId(queryspace, it->id_),it->token_,docspace,queryspace,threadid);
			}
		}
		
	} else {
		for (int johni = start2; johni < end2; johni++) {
			//		for(vector<ActiveToken>::iterator it=lbrandnew->begin(); it!=lbrandnew->end(); ++it) {
			ActiveToken* it = &(*lbrandnew2)[johni];
			google::dense_hash_set<lint> memory;
			memory.set_empty_key(0);
			if (it->token_ == NULL)
				continue;
			if (diafora == 1) {
				OnTheFly(it->token_->GetTokenstring(),2,hash1,memory,TokenId(queryspace, it->id_),it->token_,docspace,queryspace,threadid);								
			} else if (diafora == 2) {
				OnTheFly(it->token_->GetTokenstring(),2,hash0,memory,TokenId(queryspace, it->id_),it->token_,docspace,queryspace,threadid);				
			}
		}
	}
}
void QD1(int queryspace, int docspace, int threadid,int start1,int end1) {
	int diafora = abs(docspace - queryspace);
	/*Active QueryToken Lists*/
	std::vector<mple::ActiveToken> *lbrandnew3 =
	&globalmanager->indexes_[queryspace - MIN_WORD_LENGTH].qbrandnew3;
	std::vector<mple::ActiveToken> *lbrandnew2 =
	&globalmanager->indexes_[queryspace - MIN_WORD_LENGTH].qbrandnew2;
	std::vector<mple::ActiveToken> *lbrandnew1 =
	&globalmanager->indexes_[queryspace - MIN_WORD_LENGTH].qbrandnew1;
	std::vector<mple::ActiveToken> *lbrandnew0 =
	&globalmanager->indexes_[queryspace - MIN_WORD_LENGTH].qbrandnew0;
	/************/

	/* DocToken Hashes 0,-1,-2,-3 */
	google::dense_hash_map<size_t, vector<lint>> *hash0 =
	            &globalmanager->indexes_[docspace - MIN_WORD_LENGTH].dhash_[0];
	google::dense_hash_map<size_t, vector<lint>> *hash1 =
	            &globalmanager->indexes_[docspace - MIN_WORD_LENGTH].dhash_[1];
	google::dense_hash_map<size_t, vector<lint>> *hash2 =
	            &globalmanager->indexes_[docspace - MIN_WORD_LENGTH].dhash_[2];
	google::dense_hash_map<size_t, vector<lint>> *hash3 =
	            &globalmanager->indexes_[docspace - MIN_WORD_LENGTH].dhash_[3];
	/*****************************/
	if (docspace >= queryspace) {
		for (int johni = start1; johni < end1; johni++) {
//		for(vector<ActiveToken>::iterator it=lbrandnew->begin(); it!=lbrandnew->end(); ++it) {
			ActiveToken* it = &(*lbrandnew1)[johni];
			google::dense_hash_set<lint> memory;
			memory.set_empty_key(0);
			if (it->token_ == NULL)
				continue;
			if (diafora == 0) {
				OnTheFly(it->token_->GetTokenstring(),1,hash1,memory,TokenId(queryspace, it->id_),it->token_,docspace,queryspace,threadid);
			} else if (diafora == 1) {
				OnTheFly(it->token_->GetTokenstring(),0,hash1,memory,TokenId(queryspace, it->id_),it->token_,docspace,queryspace,threadid);
			}
		}
		
	} else {
		for (int johni = start1; johni < end1; johni++) {
			//		for(vector<ActiveToken>::iterator it=lbrandnew->begin(); it!=lbrandnew->end(); ++it) {
			ActiveToken* it = &(*lbrandnew1)[johni];
			google::dense_hash_set<lint> memory;
			memory.set_empty_key(0);
			if (it->token_ == NULL)
				continue;
			if (diafora == 1) {
				OnTheFly(it->token_->GetTokenstring(),1,hash0,memory,TokenId(queryspace, it->id_),it->token_,docspace,queryspace,threadid);								
			}
		}
	}
}
void QD0(int queryspace, int docspace, int threadid, int start0, int end0){
	int diafora = abs(docspace - queryspace);
	/*Active QueryToken Lists*/
	std::vector<mple::ActiveToken> *lbrandnew3 =
	&globalmanager->indexes_[queryspace - MIN_WORD_LENGTH].qbrandnew3;
	std::vector<mple::ActiveToken> *lbrandnew2 =
	&globalmanager->indexes_[queryspace - MIN_WORD_LENGTH].qbrandnew2;
	std::vector<mple::ActiveToken> *lbrandnew1 =
	&globalmanager->indexes_[queryspace - MIN_WORD_LENGTH].qbrandnew1;
	std::vector<mple::ActiveToken> *lbrandnew0 =
	&globalmanager->indexes_[queryspace - MIN_WORD_LENGTH].qbrandnew0;
	/************/

	/* DocToken Hashes 0,-1,-2,-3 */
	google::dense_hash_map<size_t, vector<lint>> *hash0 =
	            &globalmanager->indexes_[docspace - MIN_WORD_LENGTH].dhash_[0];
	google::dense_hash_map<size_t, vector<lint>> *hash1 =
	            &globalmanager->indexes_[docspace - MIN_WORD_LENGTH].dhash_[1];
	google::dense_hash_map<size_t, vector<lint>> *hash2 =
	            &globalmanager->indexes_[docspace - MIN_WORD_LENGTH].dhash_[2];
	google::dense_hash_map<size_t, vector<lint>> *hash3 =
	            &globalmanager->indexes_[docspace - MIN_WORD_LENGTH].dhash_[3];
	/*****************************/
	if (docspace >= queryspace) {
		//exact
		for (int johni = start0; johni < end0; johni++) {
//		for(vector<ActiveToken>::iterator it=lbrandnew->begin(); it!=lbrandnew->end(); ++it) {
			ActiveToken* it = &(*lbrandnew0)[johni];
			google::dense_hash_set<lint> memory;
			memory.set_empty_key(0);
			if (it->token_ == NULL)
				continue;
			if (diafora == 0) {
				OnTheFly(it->token_->GetTokenstring(),0,hash0,memory,TokenId(queryspace, it->id_),it->token_,docspace,queryspace,threadid);
			}
		}
}
}
void DQ(int queryspace, int docspace, int threadid, int start, int end) {//1
	//cout<<"Do "<<queryspace<< " "<<docspace<<endl;
	int diafora = abs(queryspace - docspace);
	std::vector<mple::ActiveToken> *ldbrandnew =
	    &globalmanager->indexes_[docspace - MIN_WORD_LENGTH].dbrandnew;

	google::dense_hash_map<size_t, vector<lint> > *hash0 =
	    &globalmanager->indexes_[queryspace - MIN_WORD_LENGTH].qhash_[0];
	google::dense_hash_map<size_t, vector<lint>> *hash1 =
	            &globalmanager->indexes_[queryspace - MIN_WORD_LENGTH].qhash_[1];
	google::dense_hash_map<size_t, vector<lint>> *hash2 =
	            &globalmanager->indexes_[queryspace - MIN_WORD_LENGTH].qhash_[2];
	google::dense_hash_map<size_t, vector<lint>> *hash3 =
	            &globalmanager->indexes_[queryspace - MIN_WORD_LENGTH].qhash_[3];
	/*****************************/
	if (queryspace >= docspace) {
		//for (vector<ActiveToken>::iterator it = ldbrandnew->begin();it != ldbrandnew->end(); ++it) {
		for (int johni = start; johni < end; johni++) {
			ActiveToken* it = &(*ldbrandnew)[johni];

			google::dense_hash_set<lint> memory;
			memory.set_empty_key(0);
			TokenId wid;
			wid.id_ = it->id_;
			wid.length_ = docspace;
			TokenInfo * docword = globalmanager->GetTokenInfo(wid);

			if (diafora == 0) {
				char res3[docspace - 2];
				res3[docspace - 3] = '\0';
				for (unsigned int i = 0; i < docspace - 2; i++) { //loop gia to -3
					strncpy(res3, docword->GetTokenstring(), i);
					for (unsigned int j = i + 1; j < docspace - 1; j++) {
						strncpy(res3 + i, docword->GetTokenstring() + i + 1,
						        j - i - 1);
						for (unsigned int k = j + 1; k < docspace; k++) {
							strncpy(res3 + j - 1,
							        docword->GetTokenstring() + j + 1,
							        k - j - 1);
							strncpy(res3 + k - 2,
							        docword->GetTokenstring() + k + 1,
							        docspace - k - 1);
							google::dense_hash_map<size_t, vector<lint> >::iterator result3 =
							    hash3->find(mur()(res3));
							if (result3 != hash3->end()) {
								//EditHamming()
								SetDimisAnapodi(memory, result3, wid, docword,
								                docspace, queryspace, false, threadid);
							}
						}
					}
				}
			} else if (diafora == 1) {
				//loop gia to -2
				char res2[docspace - 1];
				res2[docspace - 2] = '\0';
				for (unsigned int j = 0; j < docspace - 1; j++) {
					strncpy(res2, docword->GetTokenstring(), j);
					for (unsigned int k = j + 1; k < docspace; k++) {
						strncpy(res2 + j, docword->GetTokenstring() + j + 1,
						        k - j - 1);
						strncpy(res2 + k - 1, docword->GetTokenstring() + k + 1,
						        docspace - k - 1);
						google::dense_hash_map<size_t, vector<lint> >::iterator result2 =
						    hash3->find(mur()(res2));
						if (result2 != hash3->end()) {
							//EditHamming()
							SetDimisAnapodi(memory, result2, wid, docword,
							                docspace, queryspace, false, threadid);
						}
					}
				}
			} else if (diafora == 2) {
				/*on the fly*/
				//loop gia to -1
				char res1[docspace];
				res1[docspace - 1] = '\0';
				for (unsigned int k = 0; k < docspace; k++) {
					strncpy(res1, docword->GetTokenstring(), k);
					strncpy(res1 + k, docword->GetTokenstring() + k + 1,
					        docspace - k - 1);
					google::dense_hash_map<size_t, vector<lint> >::iterator result1 =
					    hash3->find(mur()(res1));
					if (result1 != hash3->end()) {
						//EditHamming()
						SetDimisAnapodi(memory, result1, wid, docword, docspace,
						                queryspace, false, threadid);
					}
				}
			} else if (diafora == 3) {
				google::dense_hash_map<size_t, vector<lint> >::iterator result =
				    hash3->find(mur()(docword->GetTokenstring()));
				if (result != hash3->end()) {
					//EditHamming()
					SetDimisAnapodi(memory, result, wid, docword, docspace,
					                queryspace, true, threadid, diafora);
				}
			}

		}
	} else {
		//for (vector<ActiveToken>::iterator it = ldbrandnew->begin();it != ldbrandnew->end(); ++it) {
		for (int johni = start; johni < end; johni++) {
			ActiveToken* it = &(*ldbrandnew)[johni];
			google::dense_hash_set<lint> memory;
			memory.set_empty_key(0);
			TokenId wid;
			wid.id_ = it->id_;
			wid.length_ = docspace;
			TokenInfo * docword = globalmanager->GetTokenInfo(wid);
			if (diafora == 0) {
				char res3[docspace - 2];
				res3[docspace - 3] = '\0';
				for (unsigned int i = 0; i < docspace - 2; i++) { //loop gia to -3
					strncpy(res3, docword->GetTokenstring(), i);
					for (unsigned int j = i + 1; j < docspace - 1; j++) {
						strncpy(res3 + i, docword->GetTokenstring() + i + 1,
						        j - i - 1);
						for (unsigned int k = j + 1; k < docspace; k++) {
							strncpy(res3 + j - 1,
							        docword->GetTokenstring() + j + 1,
							        k - j - 1);
							strncpy(res3 + k - 2,
							        docword->GetTokenstring() + k + 1,
							        docspace - k - 1);
							google::dense_hash_map<size_t, vector<lint> >::iterator result3 =
							    hash3->find(mur()(res3));
							if (result3 != hash3->end()) {
								//EditHamming()
								//cout<<"mpike ham 3 "<<endl;
								SetDimisAnapodi(memory, result3, wid, docword,
								                docspace, queryspace, false, threadid);
							}
						}
					}
				}
			} else if (diafora == 1) {
				/*on the fly*/
				char res3[docspace - 2];
				res3[docspace - 3] = '\0';
				for (unsigned int i = 0; i < docspace - 2; i++) { //loop gia to -3
					strncpy(res3, docword->GetTokenstring(), i);
					for (unsigned int j = i + 1; j < docspace - 1; j++) {
						strncpy(res3 + i, docword->GetTokenstring() + i + 1,
						        j - i - 1);
						for (unsigned int k = j + 1; k < docspace; k++) {
							strncpy(res3 + j - 1,
							        docword->GetTokenstring() + j + 1,
							        k - j - 1);
							strncpy(res3 + k - 2,
							        docword->GetTokenstring() + k + 1,
							        docspace - k - 1);
							google::dense_hash_map<size_t, vector<lint> >::iterator result2 =
							    hash2->find(mur()(res3));
							if (result2 != hash2->end()) {
								//EditHamming()
								SetDimisAnapodi(memory, result2, wid, docword,
								                docspace, queryspace, false, threadid);
							}
						}
					}
				}
			} else if (diafora == 2) {
				/*on the fly*/
				/*on the fly*/
				//loop gia to -3
				char res3[docspace - 2];
				res3[docspace - 3] = '\0';
				for (unsigned int i = 0; i < docspace - 2; i++) { //loop gia to -3
					strncpy(res3, docword->GetTokenstring(), i);
					for (unsigned int j = i + 1; j < docspace - 1; j++) {
						strncpy(res3 + i, docword->GetTokenstring() + i + 1,
						        j - i - 1);
						for (unsigned int k = j + 1; k < docspace; k++) {
							strncpy(res3 + j - 1,
							        docword->GetTokenstring() + j + 1,
							        k - j - 1);
							strncpy(res3 + k - 2,
							        docword->GetTokenstring() + k + 1,
							        docspace - k - 1);
							google::dense_hash_map<size_t, vector<lint> >::iterator result1 =
							    hash1->find(mur()(res3));
							if (result1 != hash1->end()) {
								//EditHamming()
								SetDimisAnapodi(memory, result1, wid, docword,
								                docspace, queryspace, false, threadid,
								                diafora);
							}
						}
					}
				}
			} else if (diafora == 3) {
				char res3[docspace - 2];
				res3[docspace - 3] = '\0';
				for (unsigned int i = 0; i < docspace - 2; i++) { //loop gia to -3
					strncpy(res3, docword->GetTokenstring(), i);
					for (unsigned int j = i + 1; j < docspace - 1; j++) {
						strncpy(res3 + i, docword->GetTokenstring() + i + 1,
						        j - i - 1);
						for (unsigned int k = j + 1; k < docspace; k++) {
							strncpy(res3 + j - 1,
							        docword->GetTokenstring() + j + 1,
							        k - j - 1);
							strncpy(res3 + k - 2,
							        docword->GetTokenstring() + k + 1,
							        docspace - k - 1);
							google::dense_hash_map<size_t, vector<lint> >::iterator result =
							    hash0->find(mur()(res3));
							if (result != hash0->end()) {
								//EditHamming()
								SetDimisAnapodi(memory, result, wid, docword,
								                docspace, queryspace, true, threadid,
								                diafora);
							}
						}
					}
				}
			}
		}
	}
}
#include <unistd.h>
int JobMatchPruneQD3::Run() {
	static int abb=-1;
	if(abb!=batchno) {
		//sleep(1);
		abb=batchno;
	}
	QD3(querylength, doclength, js->GetMyid(),start,end);
	return EC_SUCCESS;
}
int JobMatchPruneQD2::Run() {
	static int abb=-1;
	if(abb!=batchno) {
		//sleep(1);
		abb=batchno;
	}
	QD2(querylength, doclength, js->GetMyid(),start,end);
	return EC_SUCCESS;
}
int JobMatchPruneQD1::Run() {
	static int abb=-1;
	if(abb!=batchno) {
		//sleep(1);
		abb=batchno;
	}
	QD1(querylength, doclength, js->GetMyid(),start,end);
	return EC_SUCCESS;
}
int JobMatchPruneQD0::Run() {
	static int abb=-1;
	if(abb!=batchno) {
		//sleep(1);
		abb=batchno;
	}
	QD0(querylength, doclength, js->GetMyid(),start,end);
	return EC_SUCCESS;
}
int JobMatchPrune1::Run() {
	//cout<<"JobMatchPrune1ssssssssssss\n";
//	if(querylength<=doclength)
//		QuerysmallerDoc(querylength,doclength);
//	else
//		QuerybigerDoc(querylength,doclength);

	DQ(querylength, doclength, js->GetMyid(),start,end);
	//cout<<"JobMatchPrune1\n";
	return EC_SUCCESS;
}
//int JobMatchPrune::Run() {
//	char * buff;
//	char* token;
//	int wordsize, edit_dist, size;
//	for (int ii = start; ii < end; ++ii) {
//		TokenId wid;
//		wid.id_ = wordbatchv[length][ii] / 100;
//		wid.length_ = wordbatchv[length][ii] % 100;
//		TokenInfo * dt =  globalmanager->GetTokenInfo(wid);
//
//		buff = dt->GetTokenstring();
//		FlagIter* editid1 = dt->GetTokenEditIds1();
//		FlagIter* editid2 = dt->GetTokenEditIds2();
//		FlagIter* editid3 = dt->GetTokenEditIds3();
//		FlagIter* hamid = dt->GetTokenHamIds();
//
//		ActiveIter endoflist = globalmanager->GetActiveEndOfSizeEdit1(length + MIN_WORD_LENGTH);
//		for (ActiveIter itactive = editid1[3]; itactive != endoflist; ++itactive) {
//			//std::cout<<"1"<<std::endl;
//			if (itactive->token_ == NULL) {
//				continue;
//			}
//			token = itactive->token_->GetTokenstring();
//
//			char result = EditHamming(buff, token, wid.length_, 31);//31);
//			if (result != 0)
//				dt->AddRelation(TokenId(length + MIN_WORD_LENGTH, itactive->id_),result);
//		}
//		editid1[3] = batchiter[length][1];
//		endoflist = globalmanager->GetActiveEndOfSizeEdit2(length + MIN_WORD_LENGTH);
//		for (ActiveIter itactive = editid2[3]; itactive != endoflist; ++itactive) {
//			if (itactive->token_ == NULL) {
//				continue;
//			}
//			token = itactive->token_->GetTokenstring();
//			char result = EditHamming(buff, token, wid.length_, 63);//63);
//			if (result != 0)dt->AddRelation(TokenId(length + MIN_WORD_LENGTH, itactive->id_),result);
//		}
//		editid2[3] = batchiter[length][2];
//		endoflist = globalmanager->GetActiveEndOfSizeEdit3(length + MIN_WORD_LENGTH);
//		for (ActiveIter itactive = editid3[3]; itactive != endoflist; ++itactive) {
//			if (itactive->token_ == NULL) {
//				continue;
//			}
//			token = itactive->token_->GetTokenstring();
//			char result = EditHamming(buff, token, wid.length_, 127);
//			if (result != 0)dt->AddRelation(TokenId(length + MIN_WORD_LENGTH, itactive->id_),result);
//		}
//		editid3[3] = batchiter[length][3];
//		endoflist = globalmanager->GetActiveEndOfSizeHam(length + MIN_WORD_LENGTH);
//		for (ActiveIter itactive = hamid[0]; itactive != endoflist; ++itactive) {
//			if (itactive->token_ == NULL) {
//				continue;
//			}
//			token = itactive->token_->GetTokenstring();
//			edit_dist = HammingDistance(buff,wid.length_, token, wid.length_, 3);
//			char result = ((0b00001111 << edit_dist) & 0b00001111);
//			if (result != 0)dt->AddRelation(TokenId(length + MIN_WORD_LENGTH, itactive->id_),result);
//		}
//		hamid[0] = batchiter[length][0];
//		for (int i = -3; i < 4; ++i) {
//			wordsize = wid.length_ + i;
//			if (wordsize < 4 || i == 0) {
//				continue;
//			}
//			if (wordsize > 31) {
//				break;
//			}
//			ActiveIter endoflist = globalmanager->GetActiveEndOfSizeEdit3(wordsize);
//			for (ActiveIter itactive = editid3[i + 3]; itactive != endoflist;++itactive) {
//				if (itactive->token_ == NULL) {
//					continue;
//				}
//
//				token = itactive->token_->GetTokenstring();
//				edit_dist = levenshtein(buff, token, wid.length_, wordsize, 3);
//				char result = ((0b0111000 << edit_dist) & 0b01111111);
//				if (result != 0)dt->AddRelation(TokenId(wordsize,itactive->id_),result);
//				/*if(!strcmp("govind",buff) && !strcmp("govidhd",token))
//				{
//					std::cout<<"fuck3 "<<(int)result<<std::endl;
//				}*/
//			}
//			editid3[i+3]=batchiter[wordsize-MIN_WORD_LENGTH][3];
//			if(abs(i)<=2)
//			{
//			endoflist = globalmanager->GetActiveEndOfSizeEdit2(wordsize);
//			for (ActiveIter itactive = editid2[i + 3]; itactive != endoflist;++itactive) {
//				if (itactive->token_ == NULL) {
//					continue;
//				}
//				token = itactive->token_->GetTokenstring();
//				edit_dist = levenshtein(buff, token, wid.length_, wordsize, 2);//2);
//				char result = ((0b0111000 << edit_dist) & 0b01111111);
//				//if(!strcmp("govind",buff))
//				//{
//				//	std::cout<<"searching govind "<<std::endl;
//				//}
//				/*if(!strcmp("govind",buff) && !strcmp("govidhd",token))
//				{
//					std::cout<<"fuck2 "<<(int)result<<std::endl;
//					std::vector<TokenQmidInfo>** tokenptrs=((QueryToken*)itactive->token_)->GetPointer();
//					std::cout<<" list of qmids"<<std::endl;
//					for(int i=0;i<7;++i)
//					{
//						std::cout<<" i = "<<i<<std::endl;
//						if(tokenptrs[i]!=NULL)
//						{
//							for(std::vector<TokenQmidInfo>::iterator it=tokenptrs[i]->begin();it!=tokenptrs[i]->end();++it)
//							{
//								std::cout<<"qmid : "<<it->qmid_<<std::endl;
//							}
//						}
//					}
//
//					std::cout<<" list of doc ids"<<std::endl;
//					std::vector<DocIDInfo>* ptrrrr=dt->GetPointer();
//
//							for(std::vector<DocIDInfo>::iterator it=ptrrrr->begin();it!=ptrrrr->end();++it)
//							{
//								std::cout<<"docid : "<<it->did_<<std::endl;
//							}
//
//				}*/
//				if (result != 0)dt->AddRelation(TokenId(wordsize,itactive->id_),result);
//
//			}
//			editid2[i+3]=batchiter[wordsize-MIN_WORD_LENGTH][2];
//			}
//			if(abs(i)<=1)
//			{
//			endoflist = globalmanager->GetActiveEndOfSizeEdit1(wordsize);
//			for (ActiveIter itactive = editid1[i + 3]; itactive != endoflist;
//					++itactive) {
//				if (itactive->token_ == NULL) {
//					continue;
//				}
//				token = itactive->token_->GetTokenstring();
//				edit_dist = levenshtein(buff, token, wid.length_, wordsize, 1);//1);
//				char result = ((0b0111000 << edit_dist) & 0b01111111);
//				if (result != 0)dt->AddRelation(TokenId(wordsize,itactive->id_),result);
//				/*if(!strcmp("govind",buff) && !strcmp("govidhd",token))
//				{
//					std::cout<<"fuck1 "<<(int)result<<std::endl;
//				}*/
//			}
//			editid1[i+3]=batchiter[wordsize-MIN_WORD_LENGTH][1];
//			}
//		}
//
//		std::vector<wordsat> * curTids;
//
//		curTids = (globalmanager->GetTokenInfo(wid))->GetVector();
//		for (int i = 0; i < curTids->size(); ++i) {
//			TokenId tid = (*curTids)[i].tid;
//			TokenInfo * qt =  globalmanager->GetTokenInfo(tid);
//
//			//char tmp = 0; (FIX)
//
//			results->SetStauros((dt->GetDocidsPointer()), (qt->GetQmidsPointer()),
//					(*curTids)[i].matchtype );
//		}
//		(*dt->GetDocidsPointer()).clear();
//
//	}
//	//pthread_mutex_lock(&mv);
//	//couffnt++;
//	//pthread_mutex_unlock(&mv);
//
//	return EC_SUCCESS;
//}
void OnTheFly(char * string ,int ana,Hash* hash,google::dense_hash_set<lint> &memory,TokenId tid,TokenInfo * token,int  docspace,int queryspace,int threadid)
{
	if(ana==3)
	{
		char res3[queryspace - 2];
		res3[queryspace - 3] = '\0';
	for (unsigned int i = 0; i < queryspace - 2; i++) { //loop gia to -3
					strncpy(res3, string, i);
					for (unsigned int j = i + 1; j < queryspace - 1; j++) {
						strncpy(res3 + i, string + i + 1,
						        j - i - 1);
						for (unsigned int k = j + 1; k < queryspace; k++) {
							strncpy(res3 + j - 1,string + j + 1,k - j - 1);
							strncpy(res3 + k - 2,string + k + 1,queryspace - k - 1);
							google::dense_hash_map<size_t, vector<lint> >::iterator result =
							    hash->find(mur()(res3));
							if (result != hash->end()) {
								SetDimis(memory,result,tid,token,docspace,queryspace,false,threadid);
							}
						}
					}
				}
	}
	else if(ana==2)
	{
		char res2[queryspace - 1];
		res2[queryspace - 2] = '\0';
		for (unsigned int j = 0; j < queryspace - 1; j++) {
			strncpy(res2, string, j);
			for (unsigned int k = j + 1; k < queryspace; k++) {
				strncpy(res2 + j, string + j + 1,
						k - j - 1);
				strncpy(res2 + k - 1,
						string + k + 1,
						queryspace - k - 1);
				google::dense_hash_map<size_t, vector<lint> >::iterator result =
					hash->find(mur()(res2));
				if (result != hash->end()) {
					//EditHamming()
					SetDimis(memory, result,tid, token,docspace, queryspace, false, threadid);
					//		CheckMeion1(j,queryspace,docspace,string,hash2,it);    ?????
					//		CheckMeion1(k,queryspace,docspace,string,hash2,it);    ?????
					//CheckMeion2(i,j,queryspace,docspace,string,hash2,NULL,it);
				}
			}
		}
	}
	else if(ana==1)
	{
		char res1[queryspace];
		res1[queryspace - 1] = '\0';
		for (unsigned int k = 0; k < queryspace; k++) {
			strncpy(res1,string, k);
			strncpy(res1 + k, string + k + 1,queryspace-k - 1);
			google::dense_hash_map<size_t, vector<lint> >::iterator result =
				hash->find(mur()(res1));
			if (result != hash->end()) {
				//EditHamming()
				SetDimis(memory,result,tid,token,docspace,queryspace,false,threadid);
			}
		}
	}
	else
	{
		google::dense_hash_map<size_t, vector<lint> >::iterator result =
				    hash->find(mur()(string));
				if (result != hash->end()) {
					//EditHamming()
					SetDimis(memory,result,tid,token,docspace,queryspace,true,threadid,abs(queryspace-docspace));
				}
	}
}
} /* namespace mple */
