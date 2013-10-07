/*
 * QueryIndex.cpp
 *
 *  Created on: 27 ��� 2013
 *      Author: mple
 */
#include "Query.h"
#include "QueryInfo.h"
#include "QueryIndex.h"
#include <iostream>
#include <limits.h>
#include "../Includes/defn.h"
#include <map>
#include <cstring>
#include <cstring>
#include <utility>
using namespace std;
extern int batchno;
namespace mple {



QueryIndex::QueryIndex(TokenManager * tm) {
	  tokenmanager_ = tm;
	  qidindex.set_empty_key(0);
}

QueryIndex::~QueryIndex() {
	for (unsigned int i=0 ; i< vault_.size() ; ++i){
		delete vault_[i];
	}
}

bool QueryIndex::AddQuery(Query *Q, Qmid * qmid){
	unsigned int hashvalue = Q->hashvalue();

	Qmid target;
	target = HashLookup(Q);

	if (target == 0 && !emptycells_.empty()){//query does not exist and recycle query info
	  target = emptycells_.top();
	  emptycells_.pop();

	  qmidindex.insert(std::pair<unsigned int, Qmid>(hashvalue, target));
	  vault_[target-1]->type_ = Q->type_;
	  vault_[target-1]->dist_= Q->dist_;

	  vault_[target-1]->AddQid(Q->qid);
	  *qmid = target;
	  qidindex[Q->qid] = target;
	  TokenId tid;

	  for (int i = 0 ; i < Q->GetNTokens(); ++i){

		  tokenmanager_->AddQueryToken(Q->tokens[i], strlen(Q->tokens[i]), target, i,Q->type_, Q->dist_,batchno, &tid);
		  vault_[target-1]->AddTokenId(tid);
	  }

	  return true;
	}
	if (target == 0){ //query does not exist
	  QueryInfo * qi = new QueryInfo(Q->NTokens);
	  qi->type_ = Q->type_;
	  qi->dist_ = Q->dist_;
	  qi->AddQid(Q->qid);
	  vault_.push_back(qi);
	  *qmid = (Qmid) vault_.size();

	  qmidindex.insert(std::pair<unsigned int, Qmid>(hashvalue, *qmid));
	  qidindex[Q->qid] = *qmid;
	  TokenId tid;
	  for (int i = 0 ; i < Q->GetNTokens(); ++i){

		  tokenmanager_->AddQueryToken(Q->tokens[i], strlen(Q->tokens[i]), *qmid, i,Q->type_, Q->dist_,batchno, &tid);
		  vault_[*qmid-1]->AddTokenId(tid);
	  }

	  return true;
	}else{//query exists
	  vault_[target-1]->AddQid(Q->qid);
	  *qmid = target;
	  qidindex[Q->qid] = target;
	  return false;
	}
}

//int QueryIndex::calchash(Query* q){
//return 0;
//	}

Qmid QueryIndex::HashLookup(Query * q){//vag Fix
	unsigned int hashvalue = q->hashvalue();
	  Qmid qmid;
	  int i;
	  static int counter =0 ;
	  counter++;

	  std::pair<UQMmap::iterator, UQMmap::iterator> it = qmidindex.equal_range(hashvalue);
	  int k = 0;

	  for (; it.first != it.second; ++it.first ){

		  qmid = it.first->second;
		  	unsigned int t = (((vault_[qmid-1]->type_)*3)+vault_[qmid-1]->dist_);
	    	if(vault_[qmid-1]->type_ == q->type_ && vault_[qmid-1]->dist_ == q->dist_ && t == hashvalue/100000 && vault_[qmid-1]->GetNTokens() == q->GetNTokens() ){
	    		for (i = 0 ; i < q->GetNTokens();++i){

	    			if (strcmp((q->tokens[i]), tokenmanager_->GetTokenString(vault_[qmid-1]->tids_[i]) ) != 0){
	    				break;
	    			}
	    		}
	    		if (i ==  q->GetNTokens() ){
	    			return qmid;
	    		}
	    	}

	    	k++;
	    }

	  return 0;
	}

QueryInfo * QueryIndex::Lookup(Qmid qmid){
  return vault_[qmid-1];
}


//void QueryIndex::printall(void){
//	  cout << "+++Queryindex+++" << endl;
//	  for (unsigned int i = 0 ; i< vault_.size() ; ++i)
//	    vault_[i]->printall();
//	  cout << "---Queryindex---" << endl;
//	}

ErrorCode QueryIndex::RemoveQuery(Qid qid){
	  Qmid qmid= qidindex[qid];
	  if (qmid == 0)
	    return EC_SUCCESS;
	  vault_[qmid-1]->RemoveQid(qid);
	  if (vault_[qmid-1]->Empty()){
		for(int i=0;i<vault_[qmid-1]->GetNTokens();i++){
			tokenmanager_->RemoveTokenQmidInfo(vault_[qmid-1]->tids_[i],qmid, vault_[qmid-1]->type_, vault_[qmid-1]->dist_);
		}
		vault_[qmid-1]->clean();
	    emptycells_.push(qmid);
	  }

	  return EC_SUCCESS;
	}


}
/* namespace mple */
