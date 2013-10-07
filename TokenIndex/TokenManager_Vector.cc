#include "TokenManager_Vector.h"
#include "TokenDuplicateIndex.h"
#include "TokenStructs_Vector.h"
#include "../QueryIndex/QueryIndex.h"
#include "../QueryIndex/QueryInfo.h"
#include <string>
namespace mple {

/*----------------------------  SpaceBySize Class  --------------------------*/
SpaceBySize::SpaceBySize() {
}
void SpaceBySize::Init(lint tokenstringsize) {
	max_tokens_=0; //statistika
	batch = true;
	lastbatch_inserterdid_ = 0;
	tokenstringsize_ = tokenstringsize;
	tokens_.reserve(2048);
	for(int i=0; i<4; ++i) {
		qhash_[i].set_empty_key(0);
		dhash_[i].set_empty_key(0);
	}
}
SpaceBySize::~SpaceBySize() {
	int num = tokens_.size();
	TokenPacket* tkninfo = (num != 0) ? &tokens_.front() : NULL;
	for (int i = 0; i < num; i++) {
		delete tkninfo->token_;
		tkninfo++;
	}
}
TokenInfo * SpaceBySize::GetTokenInfo(lint id) {
	int num = tokens_.size();
	TokenPacket* ptr = (num != 0) ? &tokens_.front() : NULL;
	return ((ptr + id)->token_);
}
ErrorCode SpaceBySize::AddQueryToken(char *string, unsigned int length,
                                     Qmid qmid, lint position, MatchType mt, unsigned int dist,int batch_inserted,
                                     lint *resultid) {
	TokenInfo* token;
	tokens_.push_back(TokenPacket(token = new TokenInfo(string, length)));
	token->NewQueryToken(qmid, position, mt,dist,batch_inserted);
	*resultid = (tokens_.size() - 1);
	++max_tokens_;
	if (batch) {
		lastbatch_inserterdid_ = *resultid;
		batch = false;
	}
	qbrandnew.push_back(ActiveToken(*resultid,token));
	TokenPacket* ptr = (&tokens_.front() + (*resultid));
	ActiveToken tmp(*resultid, (ptr->token_));
	char requests=(ptr->token_)->GetRequests();
	if( requests>15 ) {
		if(requests>63) {
			qbrandnew3.push_back(ActiveToken(*resultid,token));
		} else if(requests>31) {
			qbrandnew2.push_back(ActiveToken(*resultid,token));
		} else {
			qbrandnew1.push_back(ActiveToken(*resultid,token));
		}
	} else {
		if(requests>7) {
			qbrandnew3.push_back(ActiveToken(*resultid,token));
		} else if(requests>3) {
			qbrandnew2.push_back(ActiveToken(*resultid,token));
		} else if(requests>1) {
			qbrandnew1.push_back(ActiveToken(*resultid,token));
		} else {
			qbrandnew0.push_back(ActiveToken(*resultid,token));
		}
	}
	return FRESHTOKEN;
}
ErrorCode SpaceBySize::AddQueryToken2(lint id,
                                      Qmid qmid, lint position, MatchType mt, unsigned int dist,int batch_inserted) {
	TokenPacket* ptr;
	ptr = &tokens_.front()+id;
	ptr->token_->NewQueryToken(qmid, position, mt,dist,batch_inserted);

	++max_tokens_;
	ActiveToken tmp(id, (ptr->token_));
	qbrandnew.push_back(ActiveToken(id,(ptr->token_)));
	char requests=(ptr->token_)->GetRequests();
	if( requests>15 ) {
		if(requests>63) {
			qbrandnew3.push_back(ActiveToken(id,ptr->token_));
		} else if(requests>31) {
			qbrandnew2.push_back(ActiveToken(id,ptr->token_));
		} else {
			qbrandnew1.push_back(ActiveToken(id,ptr->token_));
		}
	} else {
		if(requests>7) {
			qbrandnew3.push_back(ActiveToken(id,ptr->token_));
		} else if(requests>3) {
			qbrandnew2.push_back(ActiveToken(id,ptr->token_));
		} else if(requests>1) {
			qbrandnew1.push_back(ActiveToken(id,ptr->token_));
		} else {
			qbrandnew0.push_back(ActiveToken(id,ptr->token_));
		}
	}
	return FRESHTOKEN;
}
ErrorCode SpaceBySize::AddDocToken(char *string, unsigned int length, DocID did,int batchid, lint *resultid) {

	TokenInfo *token;
	tokens_.push_back(TokenPacket(token = new TokenInfo(string, length)));
	token->NewDocToken(did,batchid);

	*resultid = (tokens_.size() - 1);
	dbrandnew.push_back(ActiveToken(*resultid,token));
	if (batch) {
		lastbatch_inserterdid_ = *resultid;
		batch = false;
	}
	return FRESHTOKEN;
}

ErrorCode SpaceBySize::AddDocToken2(lint id, DocID did,int batchid) {

	TokenPacket* ptr;
	ptr = &tokens_.front()+id;
	ptr->token_->NewDocToken(did,batchid);
	dbrandnew.push_back(ActiveToken(id,ptr->token_));
	return OK;
}

ErrorCode SpaceBySize::RemoveTokenQmidInfo(lint id, Qmid qmid, MatchType mt,
        unsigned int dist) {
	int num;
	TokenPacket* ptr;
	num = tokens_.size();
	ptr = (num != 0) ? &tokens_.front() : NULL;
	ErrorCode err = (((ptr + id)->token_))->RemoveQmid(qmid, mt,dist);
	return err;
}
ErrorCode SpaceBySize::ClearIds() {
	int num;
	TokenPacket* ptr;
	num = tokens_.size();
	ptr = (num != 0) ? &tokens_.front() : NULL;
	for (int id = 0; id < num; id++)
		(((ptr + id)->token_))->ClearIds();
	return OK;
}
ErrorCode SpaceBySize::GetTokenString(lint id,char **result) {
	int num;
	TokenPacket* ptr;
	num = tokens_.size();
	ptr = (num != 0) ?&tokens_.front() : NULL;
	*result=((ptr+id)->token_)->GetTokenstring();
	return OK;
}
ErrorCode SpaceBySize::InsertNewQmid(lint id,Qmid qmid,lint position,MatchType mt,unsigned int dist,int batch_insert) {
	int num;
	TokenPacket* ptr;
	num = tokens_.size();
	ptr = (num != 0) ?&tokens_.front() : NULL;
	ErrorCode err=((((ptr+id))->token_))->InsertQmid(qmid,position,mt,dist,batch_insert);
	if(err == TOKENUP) {
		ActiveToken tmp(id,((ptr+id)->token_));
		char highest_request=((((ptr+id))->token_))->highest_request;
		if(highest_request==3) {
			qbrandnew3.push_back(tmp);
		} else if(highest_request==2) {
			qbrandnew2.push_back(tmp);
		} else
			qbrandnew1.push_back(tmp);
	}
	return err;
}
ErrorCode SpaceBySize::InsertNewDocid(lint id, Qmid did, int batchid) {
	int num;
	TokenPacket* ptr;
	num = tokens_.size();
	ptr = (num != 0) ?&tokens_.front() : NULL;
	if((ptr+id)->token_->memory_refreshed)
	{
		dbrandnew.push_back(ActiveToken(id,(ptr+id)->token_));
		(ptr+id)->token_->memory_refreshed=false;
	}
	return (((ptr+id)->token_))->InsertDid(did,batchid);
}
void SpaceBySize::NewBatch() {
	qbrandnew3.clear();
	qbrandnew2.clear();
	qbrandnew1.clear();
	qbrandnew0.clear();

	qbrandnew.clear();
	dbrandnew.clear();
	max_tokens_=0;
	batch = true;
	lastbatch_inserterdid_ = tokens_.size();
}
inline lint SpaceBySize::RecycleAddToken(TokenInfo * tkn) {
	tokens_.push_back(tkn);
	return tokens_.size() - 1; //return id;
}
/*----------------------------  --------------  --------------------------*/
/*----------------------------  TokenManager Class  --------------------------*/
TokenManager::TokenManager(DuplicateIndex * tokenexactmanager)
/* Input: the base number of tokens for the firsts spaces inside indexes , the exactmanager to check existance of tokens in addtoken
 *
 * Output: Nothing
 * */
{
	removed_tokens = 0;
	exactmanager_ = tokenexactmanager;
	for (lint i = 0; i <= MAX_WORD_LENGTH - MIN_WORD_LENGTH; i++) {
		indexes_[i].Init(i);
	}
}
TokenManager::~TokenManager(){}
TokenInfo * TokenManager::GetTokenInfo(TokenId globalid)
{
	TokenInfo* result;
	result = indexes_[globalid.length_ - MIN_WORD_LENGTH].GetTokenInfo(globalid.id_);
	return result;
}
void TokenManager::Restart() { //kaleitai mono meta tin NewBatch
	for(int bucket=0; bucket<28; ++bucket) {
		int ssize=indexes_[bucket].tokens_.size();
		TokenPacket * ptr=(ssize!=0)?&(indexes_[bucket].tokens_.front()):NULL;
		for(int j=0; j<ssize; ++j) {
			char requests=ptr->token_->GetRequests();
			if(requests) {
				ActiveToken tmp(j, (ptr->token_));
				indexes_[bucket].qbrandnew.push_back(ActiveToken(j,(ptr->token_)));
				if( requests>15 ) {
					if(requests>63) {
						indexes_[bucket].qbrandnew3.push_back(ActiveToken(j,ptr->token_));
					} else if(requests>31) {
						indexes_[bucket].qbrandnew2.push_back(ActiveToken(j,ptr->token_));
					} else {
						indexes_[bucket].qbrandnew1.push_back(ActiveToken(j,ptr->token_));
					}
				} else {
					if(requests>7) {
						indexes_[bucket].qbrandnew3.push_back(ActiveToken(j,ptr->token_));
					} else if(requests>3) {
						indexes_[bucket].qbrandnew2.push_back(ActiveToken(j,ptr->token_));
					} else if(requests>1) {
						indexes_[bucket].qbrandnew1.push_back(ActiveToken(j,ptr->token_));
					} else {
						indexes_[bucket].qbrandnew0.push_back(ActiveToken(j,ptr->token_));
					}
				}
			}
			if(ptr->token_->isdoct_)
			{
				//indexes_[bucket].dbrandnew.push_back(ActiveToken(j,ptr->token_));
				ptr->token_->memory_refreshed=true;
			}
			++ptr;
		}
		for(int k=0;k<4;++k)
			indexes_[bucket].dhash_[k].clear();
	}
}

char * TokenManager::GetTokenString(TokenId globalid)
/* Input: Global id
 * Output: Returns the token string from the right index
 * */
{
	char* result;
	indexes_[globalid.length_ - MIN_WORD_LENGTH].GetTokenString(globalid.id_,
	        &result);
	return result;
}
inline lint TokenManager::RecycleAddToken(TokenInfo * tkn,
        unsigned int spacesize) {
	return indexes_[spacesize].RecycleAddToken(tkn);
}

ErrorCode TokenManager::AddQueryToken(char *string, lint length, Qmid qmid,
                                      lint position, MatchType mt, unsigned int dist,int batch_inserted, TokenId * gid)
/* Input: the token with qmid info, a pointer to TokenId for returning
 * Output: The global id of the inserted token at *gid
 * */
{
	ErrorCode err;
	char tempstring[MAX_WORD_LENGTH + 1];
	strncpy(tempstring, string, length);
	tempstring[length] = '\0';
	if ((length - MIN_WORD_LENGTH) < 0 || length > MAX_WORD_LENGTH)
		return OUTOFRANGE;
	TokenId globalid;
	if (exactmanager_->LookUpTokenExactManager(tempstring, &globalid)
	        == NOTFOUND) { //check if token exists in exactmanager
		globalid.id_ = -1;
	}
	globalid.length_ = length;
	if (globalid.id_ > -1) {

		if(!(indexes_[length - MIN_WORD_LENGTH].GetTokenInfo(globalid.id_)->isqueryt_)) {

			err = indexes_[length - MIN_WORD_LENGTH].AddQueryToken2(globalid.id_,qmid,
			        position, mt, dist,batch_inserted);
		} else {
			err = indexes_[length - MIN_WORD_LENGTH].InsertNewQmid(globalid.id_,
			        qmid, position, mt, dist,batch_inserted);
		}
		*gid = globalid;
		if (err == TOKENACTIVATED) {
			++active_tokens_; //INCREASE ACTIVE TOKENS
		}
		return err;
	}

	err = indexes_[length - MIN_WORD_LENGTH].AddQueryToken(tempstring, length, qmid,
	        position, mt, dist,batch_inserted, &(globalid.id_));
	*gid = globalid;

	++active_tokens_; //INCREASE ACTIVE TOKENS
	char* temp;
	indexes_[length - MIN_WORD_LENGTH].GetTokenString(globalid.id_, &temp);
	exactmanager_->AddPair(temp, globalid);
	return err;

//return freshtoken and tokenactivated
}
void TokenManager::NewBatch() {
//dtokenidlist_.clear();
	for (int i = 0; i < 28; i++) {
		indexes_[i].NewBatch();
	}
}
ErrorCode TokenManager::AddDocToken(char *string, lint length, DocID did,int batchid,TokenId * gid)
/* Input: the token with qmid info, a pointer to TokenId for returning
 * Output: The global id of the inserted token at *gid
 * */
{
	ErrorCode err;
	char tempstring[MAX_WORD_LENGTH + 1];
	strncpy(tempstring, string, length);
	tempstring[length] = '\0';
	if ((length - MIN_WORD_LENGTH) < 0 || length > MAX_WORD_LENGTH)
		return OUTOFRANGE;
	TokenId globalid;
	if (exactmanager_->LookUpTokenExactManager(tempstring, &globalid)== NOTFOUND) { //check if token exists in exactmanager
		globalid.id_ = -1;
	}
	globalid.length_ = length;

	if (globalid.id_ > -1) {

		if(!(indexes_[length - MIN_WORD_LENGTH].GetTokenInfo(globalid.id_)->isdoct_)) {

			indexes_[length - MIN_WORD_LENGTH].AddDocToken2(globalid.id_,did,batchid);
			//indexes_[length - MIN_WORD_LENGTH].GetTokenInfo(globalid.id_)->NewDocToken(did,batchid, editbatches1, editbatches2, editbatches3,hambatches);
			*gid = globalid;
			return FRESHTOKEN;
		} else {
			err = indexes_[length - MIN_WORD_LENGTH].InsertNewDocid(globalid.id_,
			        did, batchid);

		}
		*gid = globalid;
		return err;
	}
	err = indexes_[length - MIN_WORD_LENGTH].AddDocToken(tempstring, length, did,batchid,&(globalid.id_));
	*gid = globalid;
	char* temp;
	indexes_[length - MIN_WORD_LENGTH].GetTokenString(globalid.id_, &temp);
	exactmanager_->AddPair(temp, globalid);
	return err;
}

ErrorCode TokenManager::RemoveTokenQmidInfo(TokenId globalid, Qmid qmid,
        MatchType mt, unsigned int dist)
/* Input: TokenId,qmid for deletion
 * Work: Remove the qmid if exists and deletes the list if becomes empty
 * Output: ErrorCode
 * */
{
	ErrorCode err =
	    indexes_[globalid.length_ - MIN_WORD_LENGTH].RemoveTokenQmidInfo(
	        globalid.id_, qmid, mt, dist);
	if (err == TOKENREMOVED) {
		++inactive_tokens_;
	}
	return err;
//return tokenremoved
}
void TokenManager::ClearDocIds() {
	for (int i = 0; i < 28; i++) {
		indexes_[i].ClearIds();
	}
}
/*----------------------------  --------------  --------------------------*/
} //end namespace mple
