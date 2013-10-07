#ifndef __TokenManagerVector__
#define __TokenManagerVector__
//#include "TokenExactIndex.h"
#include "TokenStructs_Vector.h"
#include "TokenDuplicateIndex.h"
#include <vector>
#include <iostream>
#include <google/dense_hash_map>
#include <google/sparsehash/libc_allocator_with_realloc.h>
#include "../MapStuff/UnorderMap.h"
//#include "../MatchStuff/Relations.h"
//#include "../QueryIndex/QueryIndex.h"
#define THRESHOLD 5

using namespace std;

namespace mple {
/* ------ Struct Active Token -------*/
struct ActiveToken {
	lint id_;
	TokenInfo* token_;
	ActiveToken(lint id,TokenInfo *token) {
		id_=id;
		token_=token;
	}
};
class QueryIndex;
struct tknhash {
	inline int operator()(TokenId ref) const {
		return ref.id_+ ref.length_*28;
	}
};
class TokenIterator;

struct TokenPacket {
	TokenInfo* token_;
	ActiveIter active_;
	TokenPacket(TokenInfo * token) {
		token_=token;
	}
};
class SpaceBySize {
private:
	lint lastbatch_inserterdid_;
	lint tokenstringsize_;
	public:
	std::vector<TokenPacket> tokens_;
	google::dense_hash_map<size_t , vector<lint>> qhash_[4];
	google::dense_hash_map<size_t , vector<lint>> dhash_[4];
		std::vector<ActiveToken> qbrandnew3;
		std::vector<ActiveToken> qbrandnew2;
		std::vector<ActiveToken> qbrandnew1;
		std::vector<ActiveToken> qbrandnew0;
		std::vector<ActiveToken> qbrandnew;
		
		std::vector<ActiveToken> dbrandnew; //docs brand new
	/* Constructors - Destructors*/

	lint max_tokens_; //statistika
	bool batch;
	SpaceBySize();
	SpaceBySize(lint tokenstringsize);
	void Init(lint tokenstringsize);
	~SpaceBySize();
	/*General Funcs*/
	TokenInfo * GetTokenInfo(lint id);
	ErrorCode GetTokenString(lint id,char **result);
	void NewBatch();
	lint RecycleAddToken(TokenInfo * tkn);

	/*Query Manager*/
	ErrorCode AddQueryToken(char *string,unsigned int,Qmid qmid,lint position,MatchType mt,unsigned int dist,int batch_inserted,lint *resultid);
	ErrorCode AddQueryToken2(lint id,Qmid qmid, lint position, MatchType mt, unsigned int dist,int batch_inserted);
	ErrorCode InsertNewQmid(lint id,Qmid qmid,lint position,MatchType mt,unsigned int dist,int batch_insert);
	ErrorCode RemoveTokenQmidInfo(lint id,Qmid qmid,MatchType mt,unsigned int dist);

	/*Document Manager*/
	ErrorCode AddDocToken(char *string,unsigned int,DocID did,int batchid,lint *resultid);
	ErrorCode AddDocToken2(lint id, DocID did,int batchid);
	ErrorCode InsertNewDocid(lint id,Qmid did,int batchid);
	ErrorCode ClearIds();
	int size() { //(FIX)
		return tokens_.size();
	}
	int GetLastId() { //(REMOVE)
		return tokens_.size(); //(FIX) borei na 8elei -1
	}

	std::vector<TokenPacket>::iterator Begin() {
		return tokens_.begin();
	}
	lint End() {
		return tokens_.size();
	}
	lint OldEnd() {
		return lastbatch_inserterdid_;
	}

	bool IsDeletedId(lint id);
	friend class TokenIterator;
	friend class TokenManager;
};
/*Token Manager , Holds MAX_WORD_LENGTH-MIN_WORD_LENGTH Indexes of each length*/
class TokenManager {
private:
	DuplicateIndex *exactmanager_;

FlagIter firstbatches[28];
public:
	SpaceBySize indexes_[MAX_WORD_LENGTH-MIN_WORD_LENGTH+1];
int BatchQtokens(int space){
	int sum=0;
	for(int i=max(0,space-MIN_WORD_LENGTH-3);i<min(space-MIN_WORD_LENGTH+3, MAX_WORD_LENGTH-MIN_WORD_LENGTH);++i)
	{
		sum+=indexes_[i].max_tokens_;
	}
	return sum;
	}
	
	lint removed_tokens; //statistika

	lint active_tokens_;  //count active
	lint inactive_tokens_; //count inactive tokens
	/* Constructors - Destructors*/
	TokenManager(DuplicateIndex * tokenexactmanager);
	~TokenManager();
	/*General Funcs*/
	TokenInfo * GetTokenInfo(TokenId globalid);
	char * GetTokenString(TokenId globalid);
	void NewBatch();

	void Restart();
	lint RecycleAddToken(TokenInfo * tkn,unsigned int spacesize);
	/*Query Manager*/
	inline FlagIter* GetFirstBatches(unsigned int space) {
		return firstbatches+(int)space-MIN_WORD_LENGTH;//&(firstbatches[space-MIN_WORD_LENGTH-3]);
	}
	ErrorCode AddQueryToken(char *string,lint length,Qmid qmid,lint position,MatchType mt,unsigned int dist,int batch_inserted,TokenId * gid);
	ErrorCode RemoveTokenQmidInfo(TokenId globalid,Qmid qmid,MatchType mt,unsigned int dist);
	void ClearQueryTokenList() {
		for(int i=0; i<28; ++i) {
			indexes_[i].tokens_.clear();
		}
	}
	/*Document Manager*/
	ErrorCode AddDocToken(char *string,lint length,DocID did,int batchid,TokenId * gid);
	void ClearDocIds();
	int size(int len) {
		return indexes_[len].size();
	}
};
}
#endif
