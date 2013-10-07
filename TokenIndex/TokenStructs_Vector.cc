#include "TokenStructs_Vector.h"
#include <string.h>
#include <stdio.h>
#include <iostream>
namespace mple {

/*----------------------------  QueryToken Class --------------------------*/
void TokenInfo::NewQueryToken(Qmid qmid, lint position, MatchType mt,
		unsigned int dist, int batch_inserted)
		/* Input: string and pointer to the qmid list
		 * Allocs: 1
		 * */
		{
	highest_request=0;
	isqueryt_ = true;
	batch_inserted_ = batch_inserted;  //batch inserted
	memset(querypointer_, 0, sizeof(querypointer_));
	requests_ = 0;
	InsertQmid(qmid, position, mt, dist, batch_inserted);
	//memory_refreshed=false;	
}
ErrorCode TokenInfo::InsertQmid(Qmid qmid, lint position, MatchType mt,
		unsigned int dist, int batch_insert)
		/* Input: qmid ,position
		 * Work: add to list the new info
		 * */
		{
	TokenQmidInfo info;
	info.qmid_ = qmid;
	info.position_ = position;
	switch (mt) {
	case MT_EXACT_MATCH:
		requests_ = requests_ | 0x01;
		if (querypointer_[0] == NULL)
			querypointer_[0] = new std::vector<TokenQmidInfo>();
		querypointer_[0]->push_back(info);
		break;
	case MT_HAMMING_DIST:
		if (dist == 1) {
			requests_ = requests_ | 0x02;
			if (querypointer_[1] == NULL)
				querypointer_[1] = new std::vector<TokenQmidInfo>();
			querypointer_[1]->push_back(info);
		} else if (dist == 2) {
			requests_ = requests_ | 0x04;
			if (querypointer_[2] == NULL)
				querypointer_[2] = new std::vector<TokenQmidInfo>();
			querypointer_[2]->push_back(info);
		} else if (dist == 3) {
			requests_ = requests_ | 0x08;
			if (querypointer_[3] == NULL)
				querypointer_[3] = new std::vector<TokenQmidInfo>();
			querypointer_[3]->push_back(info);
		}
		break;
	case MT_EDIT_DIST:
		if (dist == 1) {
			requests_ = requests_ | 0x10;
			if (querypointer_[4] == NULL)
				querypointer_[4] = new std::vector<TokenQmidInfo>();
			querypointer_[4]->push_back(info);
		} else if (dist == 2) {
			requests_ = requests_ | 0x20;
			if (querypointer_[5] == NULL)
				querypointer_[5] = new std::vector<TokenQmidInfo>();
			querypointer_[5]->push_back(info);
		} else if (dist == 3) {
			requests_ = requests_ | 0x40;
			if (querypointer_[6] == NULL)
				querypointer_[6] = new std::vector<TokenQmidInfo>();
			querypointer_[6]->push_back(info);
		}
		break;
	default:
		return ERROR;
	}
	old_high=highest_request;
	if((requests_ & 0b01001000) && highest_request<3)
	{
		highest_request=3;
			return TOKENUP;
	}
	else if((requests_ & 0b00100100) && highest_request<2)
		{
		highest_request=2;
			return TOKENUP;
	}
	else if((requests_ & 0b00010010) && highest_request<1)
		{
		highest_request=1;
		return TOKENUP;
	}
	return OK;
}

ErrorCode TokenInfo::RemoveQmid(Qmid qmid, MatchType mt, unsigned int dist)
{
	bool found = false;
	unsigned int bucket = (mt == 0) ? 0 : ((mt == 1) ? dist : 3 + dist);
	lint num = querypointer_[bucket]->size();
	TokenQmidInfo *ptr = (num != 0) ? &(querypointer_[bucket]->front()) : NULL;
	for (int i = 0; i < num; i++) {
		if (ptr->qmid_ == qmid) {
			querypointer_[bucket]->erase(querypointer_[bucket]->begin() + i);
			found = true;
			break;
		}
		ptr++;
	}
	if (found) {
		if (querypointer_[bucket]->size() == 0) {
			switch (bucket) {
			case 0:
				requests_ = requests_ & 0xFE;
				delete querypointer_[0];
				querypointer_[0] = NULL;
				break;
			case 1:
				requests_ = requests_ & 0xFD;
				delete querypointer_[1];
				querypointer_[1] = NULL;
				break;
			case 2:
				requests_ = requests_ & 0xFB;
				delete querypointer_[2];
				querypointer_[2] = NULL;
				break;
			case 3:
				requests_ = requests_ & 0xF7;
				delete querypointer_[3];
				querypointer_[3] = NULL;
				break;
			case 4:
				requests_ = requests_ & 0xEF;
				delete querypointer_[4];
				querypointer_[4] = NULL;
				break;
			case 5:
				requests_ = requests_ & 0xDF;
				delete querypointer_[5];
				querypointer_[5] = NULL;
				break;
			case 6:
				requests_ = requests_ & 0xBF;
				delete querypointer_[6];
				querypointer_[6] = NULL;
				break;
			}
		}
		if(requests_==0)
		{
			highest_request=0;
		}

		return OK;
	} else
		return NOTFOUND;
}
/*----------------------------  --------------  --------------------------*/

/*----------------------------  DocToken Class  --------------------------*/

void TokenInfo::NewDocToken(DocID did, int batchid) {
	isdoct_ = true;
	docpointer_ = new std::vector<DocIDInfo>();
	lastbatch_ = -1;
	InsertDid(did, batchid);
}
ErrorCode TokenInfo::InsertDid(DocID did, int batchid) {
	DocIDInfo info;
	info.did_ = did;
	if (docpointer_->size()) {
		if (docpointer_->back().did_ != did) {
			docpointer_->push_back(info);
		}
	} else {
		docpointer_->push_back(info);
	}
	if (batchid == lastbatch_) {
		lastbatch_ = batchid;
		return TOKENEXISTSINBATCH;
	}
	lastbatch_ = batchid;
	return OK;
}
/*----------------------------  --------------  --------------------------*/
}	//end namespace mple
