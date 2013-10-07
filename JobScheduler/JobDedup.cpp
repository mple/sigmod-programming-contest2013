/*
 * JobDedup.cpp
 *
 *  Created on: Mar 29, 2013
 *      Author: johnchronis
 */

#include "JobDedup.h"
extern mple::TokenManager * globalmanager;
//extern mple::TokenManager * docmanager;
extern mple::JobScheduler* js;;
extern google::dense_hash_map<int, bool> wordbatch;
extern std::vector<int>* wordbatchv;
extern std::vector<DocID> globaldocs;
extern int batchno;
;
extern pthread_mutex_t mtx_empty_spot;
extern pthread_cond_t cond_empty_spot;
extern std::queue<int> empty_spot;
extern vector<int> doctokens;
;
extern char** matchdocjs_buff;

namespace mple {

JobDedup::~JobDedup() {
	// TODO Auto-generated destructor stub
}

int JobDedup::Run() {


	char *cur_doc_str = (char*) matchdocjs_buff[spot];
	char *buff;
	google::dense_hash_map<const char *, bool, mur, eqstr> docdupli_;
	docdupli_.set_empty_key("");
	int i = 0, wl;
	bool doc_end = false;
	if (cur_doc_str[0] == '\0') {
		doc_end = true;
	}
	//int kara=0;
	//int dkara = 0;
	//if(dn!=dkara){
	//	dkara=dn;
	//	kara=0;
	//}


	while (!doc_end) {        //every word in a doc
		wl = 0;
		while (cur_doc_str[i] != '\0' && cur_doc_str[i] == ' ') {
			++i;
		}
		buff = &cur_doc_str[i];
		while (cur_doc_str[i] != '\0' && cur_doc_str[i] != ' ') {
			++i;
			++wl;
		}
		if (wl == 0) {
			continue;
		}
		if (cur_doc_str[i] == '\0')
			doc_end = true;
		i++;

		mple::TokenId temp;
		mple::ErrorCode ecode = globalmanager->AddDocToken(buff, wl, dn+1,batchno, &temp);
		if (ecode != mple::TOKENEXISTSINBATCH) {
			if (false){//kara<5 && dn==0) {
				//kara++;
				//JobMatchPruneDedup *J = new mple::JobMatchPruneDedup( globalmanager->GetTokenInfo(temp),temp.length_);
				//js->Schedule(J);
				doctokens.push_back(temp.key());
//				cout<<"ours ------ "<<((DocToken *) docmanager->GetTokenInfo(temp))->GetTokenstring()<<endl;
			} else {
				wordbatchv[wl - MIN_WORD_LENGTH].push_back(temp.key());
				//cout<<"not ours ------ "<<((DocToken *) docmanager->GetTokenInfo(temp))->GetTokenstring()<<endl;
			}
			//wordbatch[temp.key()] = true;
		}
	}
	globaldocs.push_back(dn+1);
	pthread_mutex_lock(&mtx_empty_spot);
	empty_spot.push(spot);
	pthread_cond_signal(&cond_empty_spot);
	pthread_mutex_unlock(&mtx_empty_spot);

}

} /* namespace mple */
