/*
 * QueryInfo.h
 *
 *  Created on: 27 ��� 2013
 *      Author: mple
 */

#ifndef QUERYINFO_H_
#define QUERYINFO_H_
#include "../Includes/defn.h"
#include "QidsVault.h"
#include "../Includes/core.h"
#include "../TokenIndex/TokenStructs_Vector.h"

namespace mple {



class QueryInfo {
	QidsVault vault_;
	unsigned char NTokens_;

public:
	TokenId tids_[MAX_QUERY_WORDS+1];
	MatchType type_;
	char dist_;
	ErrorCode AddQid(Qid qid) {
		return vault_.Add(qid);
	}
	ErrorCode RemoveQid(Qid qid) {
		return vault_.Remove(qid);
	}
	bool Empty(void) {
		return vault_.Empty();
	}
	void printall(void) {
		vault_.printall();
	}
	QidsVaultIterator GiveIterator(){
		return vault_.GiveIterator();
	}
	void AddTokenId(TokenId tid) {

		tids_[NTokens_].length_ = tid.length_;
		tids_[NTokens_].id_ = tid.id_;
		NTokens_++;
	}
	char GetNTokens(){
		return NTokens_;
	}

	void clean(){
		NTokens_ = 0;
	}
	~QueryInfo() {

	}
	QueryInfo(int ntokens){
		type_ = MT_EXACT_MATCH;//mono kai mono gia na figei to warning
		NTokens_ = 0;
		dist_ = 0;
	}

};



} /* namespace mple */
#endif /* QUERYINFO_H_ */
