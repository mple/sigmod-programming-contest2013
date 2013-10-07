#ifndef __TokenStructsVector__
#define __TokenStructsVector__
#include "core.h"
#include "defn.h"
#include <vector>
#include <list>
#include <cstring>
#include <iterator>
#include "../Includes/defn.h"
#include <google/dense_hash_map>
#include <google/dense_hash_set>
#include <sparsehash/sparse_hash_map>
#include <google/sparsehash/libc_allocator_with_realloc.h>
#include "../top_impl.h"
#include "../MapStuff/UnorderMap.h"
#define MIN_WORD_LENGTH 4
#define MAX_WORD_LENGTH 31
#define pragma pack(4);

namespace mple {

enum ErrorCode {
	EC_SUCCESS, //core.h
	EC_NO_AVAIL_RES, //core.h
	EC_FAIL, //core.h

	MEMORYERROR,
	OUTOFRANGE,

	ERROR,
	NOTFOUND,
	FRESHTOKEN,
	TOKENEXISTS,
	
	TOKENEXISTSINBATCH,
	TOKENREMOVED,
	
	
	TOKENREMOVED_EDIT1,
	TOKENREMOVED_EDIT2,
	TOKENREMOVED_EDIT3,
	TOKENREMOVED_HAM,
	TOKENGONEHAMMING1,
	TOKENGONEHAMMING2,
	TOKENGONEHAMMING3,
	TOKENGONEEDIT2_3,
	TOKENGONEEDIT1_3,
	TOKENGONEEDIT1_2,
	
	TOKENACTIVATED,
	TOKENGONEEDIT1,
	TOKENGONEEDIT2,
	TOKENGONEEDIT3,
	TOKENGONEEDIT3_1,
	TOKENGONEEDIT3_2,
	TOKENGONEEDIT2_1,
	
	TOKENUP,
	
	
	
	
	OK,
	NOTEXPECTED
};

struct ActiveToken;
typedef std::list<ActiveToken>::iterator FlagIter;
typedef std::list<ActiveToken>::iterator ActiveIter;
typedef int lint;
typedef google::dense_hash_map<size_t,std::vector<lint> > Hash;

/*Struct Global Id (length,id)*/
struct TokenId {
	lint length_;
	lint id_;
	TokenId(lint length,lint id)
	{
		length_=length;
		id_=id;
	}
	TokenId(){}
	bool operator==(const struct TokenId & ref) const{
		return ((this->length_ == ref.length_) && (this->id_ == ref.id_));
	}
	bool operator==(void * ref) {
		return ((this->length_ < 0) || (this->id_ < 0));
	}
	bool operator!=(void * ref) {
		return ((this->length_ > -1) && (this->id_ > -1));
	}
	int key(void){
		return id_ * 100 + length_;
	}

};
struct wordsat{
	TokenId tid;
	char matchtype;
};
struct TokenQmidInfo {
	Qmid qmid_;
	lint position_;
	bool operator==(struct TokenQmidInfo& ref) {
		return (this->qmid_ == ref.qmid_);
	}
};
struct DocIDInfo {
	DocID did_;
};
/*-------------     TokenInfo   ---------------*/
class TokenInfo {
	protected:
	//pthread_mutex_t mtx_resss;
	char tokenstring_[MAX_WORD_LENGTH + 1];  //the string
	/*DocStuff*/
	std::vector<DocIDInfo>* docpointer_;
	/**/
	/*QueryStuff*/
	char requests_; //requests of each type
	std::vector<TokenQmidInfo>* querypointer_[7];
	/**/
	public:
	bool memory_refreshed;
	bool isqueryt_;
	bool isdoct_;
	int batch_inserted_;  //actual batch the token inserted

	char * GetTokenstring() {
		return tokenstring_;
	}
	TokenInfo(char* tokenstring,unsigned int length) {
		strncpy(tokenstring_, tokenstring,length);
		memory_refreshed=false;
		tokenstring_[length]='\0';
		isqueryt_=false;
		isdoct_=false;
		docpointer_=NULL;
		requests_=0;
		memset(querypointer_,0,sizeof(querypointer_));
	}
	virtual ~TokenInfo() {
		if(isqueryt_)
		for(int i=0; i<7; i++) {
		if(querypointer_[i]!=NULL)
			delete querypointer_[i];
	}
	if(isdoct_)
		delete docpointer_;
	}
	
	/*DocStuff*/
	int lastbatch_;
	void NewDocToken(DocID did,int batchid);
	ErrorCode InsertDid(DocID did,int batchid);
	inline std::vector<DocIDInfo> * GetDocidsPointer() {
		return docpointer_;
	}
	inline void ClearIds() {
		docpointer_->clear();
	}
	/***end DocStuff*/
	/*QueryStuff*/
	char old_high;
	char highest_request;
	void NewQueryToken(Qmid qmid, lint position, MatchType mt,unsigned int dist,int batch_inserted);
	ErrorCode InsertQmid(Qmid qmid, lint position, MatchType mt,
			unsigned int dist,int batch_insert);
	ErrorCode RemoveQmid(Qmid qmid, MatchType mt, unsigned int dist);

	inline std::vector<TokenQmidInfo> ** GetQmidsPointer() {
		return querypointer_;
	}
	inline char GetRequests() {
		return requests_;
	}
	/**/
};
}
#endif
